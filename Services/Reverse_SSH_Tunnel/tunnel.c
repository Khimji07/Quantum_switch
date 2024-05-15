#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cJSON.h"
#include <curl/curl.h>
#include <time.h>


#define LOG_FILE "/var/log/tunnel/current"

// Log a message to a file with a custom prefix
void log_message(const char *prefix, const char *message)
{
    FILE *fp = fopen(LOG_FILE, "a"); // Open the log file in append mode
    if (fp == NULL)
    {
        perror("Error opening log file");
        return;
    }

    // Get the current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    // Write the timestamp, prefix, and message to the log file
    fprintf(fp, "[%s] %s %s\n", time_str, prefix, message);
    fclose(fp);
}

// Log only message
void log_only_message(const char *message)
{
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file != NULL)
    {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
    else
    {
        perror("Error opening log file");
    }
}

// Get the rudder url from sw-config.json
char *get_rudder_url(const char *json_filename)
{
    // Open the JSON file
    FILE *fp = fopen(json_filename, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read the JSON file into a buffer
    char *json_buffer = (char *)malloc(file_size + 1);
    fread(json_buffer, 1, file_size, fp);
    json_buffer[file_size] = '\0';
    fclose(fp);

    // Parse the JSON string
    cJSON *root = cJSON_Parse(json_buffer);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        cJSON_Delete(root);
        free(json_buffer);
        return NULL;
    }

    // Extract the value of "rudder_url"
    cJSON *wizard = cJSON_GetObjectItemCaseSensitive(root, "wizard");
    if (cJSON_IsObject(wizard))
    {
        cJSON *rudder_url = cJSON_GetObjectItemCaseSensitive(wizard, "rudder_url");
        if (cJSON_IsString(rudder_url) && (rudder_url->valuestring != NULL))
        {
            char *url = strdup(rudder_url->valuestring); // Allocate memory for the URL
            cJSON_Delete(root);
            free(json_buffer);
            return url; // Return the extracted URL
        }
    }

    // Cleanup
    cJSON_Delete(root);
    free(json_buffer);
    return NULL;
}

// Generel function for read the file with it's permission
int open_file(const char *fname, const char *op_type)
{
    FILE *fp = fopen(fname, op_type);
    if (!fp)
    {
        perror("Error opening file");
        return -1;
    }
    fclose(fp);
    return 0;
}

// Get the serial number form .registry file
char *get_serial_number()
{
    FILE *fp;
    char line[256];
    const char *key = "0x1004";
    char *serial_number = NULL;

    // Open the file
    fp = fopen("/mnt/flash/system/.registry", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), fp))
    {
        // Find the key in the line
        char *pos = strstr(line, key);
        if (pos != NULL)
        {
            // Extract the serial number
            if (sscanf(pos, "%*[^=]=%s", line) == 1)
            {
                serial_number = strdup(line); // Copy the serial number
                break;                        // Exit loop after finding the serial number
            }
        }
    }

    // Close the file
    fclose(fp);

    // Return the serial number
    return serial_number;
}

// Get the Switch SSH Key
char *key_get()
{
    DIR *dir = opendir("/mnt/flash/external/ssh");
    if (!dir)
    {
        mkdir("/mnt/flash/external/ssh", 0755);
    }

    if (open_file("/mnt/flash/external/ssh/id_dropbear", "r") != 0)
    {
        char command[512]; // Increased size to 512
        sprintf(command, "dropbearkey -t rsa -f /mnt/flash/external/ssh/id_dropbear 2>/dev/null");

        FILE *pipe = popen(command, "r");
        if (pipe == NULL)
        {
            printf("Error executing command.\n");
            return NULL;
        }

        pclose(pipe);

        sprintf(command, "cp /mnt/flash/external/ssh/id_dropbear /root/.ssh/ 2>/dev/null");
        system(command);
    }

    char output[2048]; // Increased size to handle longer keys
    char command[512]; // Increased size to 512
    sprintf(command, "dropbearkey -y -f /mnt/flash/external/ssh/id_dropbear | grep -o 'ssh-rsa.*'");
    FILE *pipe = popen(command, "r");
    if (pipe == NULL)
    {
        printf("Error executing command.\n");
        return NULL;
    }

    size_t len = 0;
    char *public_key = NULL;
    while (fgets(output + len, sizeof(output) - len, pipe) != NULL)
    {
        len = strlen(output);
        if (len > 0 && output[len - 1] == '\n')
        {
            output[len - 1] = '\0'; // Remove the newline character
            break;                  // Exit the loop when a complete line is read
        }
    }

    if (len > 0)
    {
        public_key = strdup(output);
    }

    pclose(pipe);

    return public_key;
}

// Callback function to handle the response from the server
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    memcpy(userp, contents, realsize);
    return realsize;
}

// Debug callback function to print the request
int debug_callback(CURL *curl, curl_infotype type, char *data, size_t size, void *userptr)
{
    (void)curl;
    (void)userptr;
    if (type == CURLINFO_DATA_OUT)
    {
        printf("Request:\n%.*s\n", (int)size, data);
    }
    return 0;
}

// Function to send a request with given data
char *send_request(const char *data, const char *url)
{
    CURL *curl;
    CURLcode res;
    char *response_buffer = NULL;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a handle for the request
    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = NULL;

        // Set the request headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer AccessToken");
        headers = curl_slist_append(headers, "accesstoken: e851df65-7a3f-11ee-9926-90de00353e50");

        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the request headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the request data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Set the debug callback function to print the request
        // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback); // For Debuging the Curl request
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // For enable verbos mode

        // Create a buffer to store the response
        response_buffer = (char *)malloc(4096); // Adjust buffer size as needed
        if (!response_buffer)
        {
            fprintf(stderr, "Error allocating memory for response buffer\n");
            return NULL;
        }
        memset(response_buffer, 0, 4096);

        // Set the buffer as the write target
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        // Disable SSL verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(response_buffer);
            response_buffer = NULL;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return response_buffer;
}

// Parse the API respoce and seprate the Data
void parse_api_response(const char *response, char **key, char **port)
{
    cJSON *root = cJSON_Parse(response);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }

    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    if (cJSON_IsObject(data))
    {
        cJSON *key_json = cJSON_GetObjectItemCaseSensitive(data, "key");
        cJSON *port_json = cJSON_GetObjectItemCaseSensitive(data, "port");
        if (cJSON_IsString(key_json) && cJSON_IsString(port_json))
        {
            *key = strdup(key_json->valuestring);
            *port = strdup(port_json->valuestring);
        }
    }

    cJSON_Delete(root);
}

// Set the Cloud ssh key into switch
void key_set(char *key)
{
    DIR *dir;
    FILE *fp;

    dir = opendir("/mnt/flash/external/ssh");
    if (!dir)
    {
        if (mkdir("/mnt/flash/external/ssh", 0755) != 0)
        {
            printf("Error creating directory in /mnt/flash/external/ssh\n");
            return;
        }
        // printf("Directory created in /mnt/flash/external/ssh\n");
        log_only_message("Directory created in /mnt/flash/external/ssh\n");
    }
    else
    {
        closedir(dir);
    }

    fp = fopen("/mnt/flash/external/ssh/authorized_keys", "a");
    if (!fp)
    {
        printf("Error opening /mnt/flash/external/ssh/authorized_keys\n");
        return;
    }

    fputs(key, fp);
    fclose(fp);

    dir = opendir("/root/.ssh");
    if (!dir)
    {
        if (mkdir("/root/.ssh", 0755) != 0)
        {
            printf("Error creating directory in /root/.ssh\n");
            return;
        }
        // printf("Directory created in /root/.ssh\n");
        log_only_message("Directory created in /root/.ssh");
    }
    else
    {
        closedir(dir);
    }

    fp = fopen("/root/.ssh/authorized_keys", "w");
    if (!fp)
    {
        printf("Error opening /root/.ssh/authorized_keys\n");
        return;
    }

    fputs(key, fp);
    fclose(fp);

}

// Establish the reverse tunnel switch to cloud
void tunnel(const char *domain, const char *port)
{
    char *cmd = malloc(200);
    FILE *fp;

    sprintf(cmd, "ssh -y -f -N -i /mnt/flash/external/ssh/id_dropbear -R localhost:%s:127.0.0.1:22222 switch-control@%s -p 2232 >/dev/null 2>&1", port, domain);

    fp = popen(cmd, "r");
    if (!fp)
    {
        perror("Error executing command");
        return;
    }

    pclose(fp);
    free(cmd);
}

// Store the port value in a file
void store_port(const char *port)
{
    FILE *fp = fopen("/mnt/flash/external/persistent/stand_tunnel_port", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }

    fputs(port, fp);
    fclose(fp);
}

// Main function
int main()
{
    char *rudderurl = get_rudder_url("/mnt/flash/sw-config.json");
    // printf("%s\n", rudderurl);
    log_message("Rudderurl :", rudderurl);

    char *serial_number = get_serial_number();
    // printf("%s\n", serial_number);
    log_message("Serialnumber :", serial_number);

    char *switchsshkey = key_get();
    // printf("%s\n", switchsshkey);
    log_message("SwitchSSHkey :",switchsshkey);

    char data[1024];
    // snprintf(data, sizeof(data), "{\"cloud_identity\": 6, \"serial_number\": \"%s\", \"device_type\": \"SWITCH\", \"ssh_key\": \"%s\"}", serial_number, switchsshkey);
    snprintf(data, sizeof(data), "{\"serial_number\": \"%s\", \"device_type\": \"SWITCH\", \"ssh_key\": \"%s\"}", serial_number, switchsshkey);

    const char *url_path = "/api/v4/sshkeygenerator";
    char *url = (char *)malloc(strlen(rudderurl) + strlen(url_path) + 1);
    if (url == NULL)
    {
        fprintf(stderr, "Error allocating memory for URL\n");
        return -1;
    }

    strcpy(url, rudderurl);
    strcat(url, url_path);

    char *response = send_request(data, url);
    log_message("API_Response :",response);

    char *key = NULL;  // store the ssh key which we get in api response
    char *port = NULL; // store the port which we get in api response
    parse_api_response(response, &key, &port);

    log_message("Key :",key);
    log_message("Port :",port);

    // printf("Key: %s\n", key);
    // printf("Port: %s\n", port);

    // Store the port value in a file
    store_port(port);

    char *domain = rudderurl;
    size_t url_len = strlen(rudderurl);
    if (url_len >= 8 && strncmp(rudderurl, "https://", 8) == 0)
    {
        domain = rudderurl + 8; // Move the pointer to the position after "https://"
    }
    // printf("The domain name is %s\n", domain);

    key_set(key);
    tunnel(domain, port);

    free(url);
    free(rudderurl);
    free(serial_number);
    free(switchsshkey);
    free(response);
    free(key);
    free(port);

    return 0;
}
