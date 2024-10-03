#include <stdio.h>
#include <string.h>
#include <libssh/libssh.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

void base64_decode(const char* b64message, unsigned char** buffer, size_t* length) {
    BIO *bio, *b64;
    int decodeLen = strlen(b64message);
    *buffer = (unsigned char*)malloc(decodeLen);
    memset(*buffer, 0, decodeLen);

    bio = BIO_new_mem_buf(b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines
    *length = BIO_read(bio, *buffer, decodeLen);
    BIO_free_all(bio);
}

char *base64_encode(const unsigned char *buffer, size_t length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    char *b64text = (char *)malloc((bufferPtr->length + 1) * sizeof(char));
    memcpy(b64text, bufferPtr->data, bufferPtr->length);
    b64text[bufferPtr->length] = '\0';

    return b64text;
}

void generate_pbkdf2_key(const char *password, const unsigned char *salt, int salt_len, int iterations, int keylen, unsigned char *output) {
    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, salt_len, iterations, EVP_sha512(), keylen, output);
}

int main() {
    char password[256];
    const char *salt_b64 = "MT7pAgt/HQJrRoh/fGqPgA==";
    int iterations = 10000;
    int keylen = 512 / 8;
    unsigned char key[keylen];
    unsigned char *salt;
    size_t salt_len;

    // Prompt user for password
    printf("Enter Serial Number: ");
    fgets(password, sizeof(password), stdin);

    // Remove the newline character from the end if present
    size_t password_len = strlen(password);
    if (password_len > 0 && password[password_len - 1] == '\n') {
        password[password_len - 1] = '\0';
    }

    // Decode the salt from base64
    base64_decode(salt_b64, &salt, &salt_len);

    // Generate the derived key
    generate_pbkdf2_key(password, salt, (int)salt_len, iterations, keylen, key);

    // Encode the derived key in base64 and print it
    char *b64key = base64_encode(key, keylen);
    // printf("Derived Key: %s\n", b64key);

    // Extract the specific characters to form the new password
    char new_password[13] = {0}; // 12 characters + null terminator
    int positions[] = {0, 7, 14, 21, 28, 35, 42, 49, 56, 63, 70, 77}; // 1st, 8th, 15th, etc.

    for (int i = 0; i < 12; i++) {
        new_password[i] = b64key[positions[i]];
    }

    // Print the generated password
    // printf("Generated Password: %s\n", new_password);

    ssh_session session;
    ssh_channel channel;
    int rc;
    char remote_host[256];
    const char *user = "root"; // Replace with your SSH username
    char *ssh_password = new_password; // Use a different variable name
    char buffer[256];
    unsigned int nbytes;
    int port = 22222; // Default SSH port

    // Get the remote host from user input
    printf("Enter Switch IP Address: ");
    if (fgets(remote_host, sizeof(remote_host), stdin) == NULL) {
        perror("fgets");
        return 1;
    }

    // Remove newline character from input if present
    size_t remote_host_len = strlen(remote_host);
    if (remote_host_len > 0 && remote_host[remote_host_len - 1] == '\n') {
        remote_host[remote_host_len - 1] = '\0';
    }

    session = ssh_new();
    if (session == NULL) {
        fprintf(stderr, "Error creating SSH session\n");
        return 1;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, remote_host);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, user);

    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to remote host: %s\n", ssh_get_error(session));
        ssh_free(session);
        return 1;
    }

    rc = ssh_userauth_password(session, NULL, ssh_password);
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s \nPlease Verify the Serial number and Switch IP address \n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    channel = ssh_channel_new(session);
    if (channel == NULL) {
        fprintf(stderr, "Error creating channel\n");
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error opening channel: %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    int ram_value, flash_value, pkt_buffer;
    char license[256];
    char command[1024];
    
    // Prompt user to input RAM and Flash values
    printf("Enter the RAM value (ex: 1024): ");
    scanf("%d", &ram_value);

    printf("Enter the Flash value (ex: 1024): ");
    scanf("%d", &flash_value);

    printf("Enter the Packet Buffer Memory value (ex: 4): ");
    scanf("%d", &pkt_buffer);

    // Clear the input buffer before reading a full line
    getchar(); // To consume the newline character left by the previous scanf

    printf("Enter the Add On License Value (ex: QN-225-4DSPU, QN-225-4DSPU, QN-225-NSU2, QN-225-SEC-U , NA = No License): ");
    fgets(license, sizeof(license), stdin); // Read the entire line

    // Remove newline character from license if present
    license[strcspn(license, "\n")] = 0; // This will replace the newline character with null terminator
    
    // sprintf(command, "echo %d > /mnt/flash/external/persistent/.ram && echo %d > /mnt/flash/external/persistent/.flash && echo %d > /mnt/flash/external/persistent/pkt_buffer && sh /usr/bin/system-info", ram_value, flash_value, pkt_buffer);

    sprintf(command, 
        "mkdir -p /mnt/flash/external/persistent && sleep 1 && "
        "echo %d > /mnt/flash/external/persistent/.ram && sleep 1 && "
        "echo %d > /mnt/flash/external/persistent/.flash && sleep 1 && "
        "echo %d > /mnt/flash/external/persistent/pkt_buffer && sleep 1 && "
        "echo \"%s\" > /mnt/flash/external/persistent/add_on_licence && sleep 1 && "
        "sh /usr/bin/system-info && sleep 1", 
        ram_value, flash_value, pkt_buffer, license);


    rc = ssh_channel_request_exec(channel, command);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error executing command: %s\n", ssh_get_error(session));
    }
    

    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        printf("%.*s", nbytes, buffer);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    // Now, let's create a second session to execute the "cat /mnt/flash/system-info" command
    ssh_channel second_channel = ssh_channel_new(session);
    if (second_channel == NULL) {
        fprintf(stderr, "Error creating second channel\n");
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    rc = ssh_channel_open_session(second_channel);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error opening second channel: %s\n", ssh_get_error(session));
        ssh_channel_free(second_channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    printf ("\nAll Parameter set successfully\n\n");
    printf ("\nSeted Parameter as below in Switch\n\n");


    // Execute the second command: "cat /mnt/flash/system-info"
    const char *second_command = "cat /mnt/flash/system-info";
    rc = ssh_channel_request_exec(second_channel, second_command);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error executing second command: %s\n", ssh_get_error(session));
    }

    // Read the output of the second command
    while ((nbytes = ssh_channel_read(second_channel, buffer, sizeof(buffer), 0)) > 0) {
        printf("%.*s", nbytes, buffer);
    }

    // Close and free the second channel
    ssh_channel_send_eof(second_channel);
    ssh_channel_close(second_channel);
    ssh_channel_free(second_channel);

    ssh_disconnect(session);
    ssh_free(session);

    // Free allocated memory
    free(b64key);
    free(salt);

    printf("License Activated Successfully\n");

    return 0;
}



