#include <stdio.h>
#include <stdlib.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <dlfcn.h>
#include <string.h>

typedef void (*SnmpFunction)(void);

void dhcpserver_dns() {
    const char *host = "172.16.100.46";
    const char *community = "public";
    char dns_server[100][255];
    char dhcp_pools[100][255];
    int dns_count = 0, dhcp_count = 0; 
    oid dnsConfigOid[] = {1, 3, 6, 1, 4, 1, 89, 38, 47, 1, 1};
    netsnmp_session session, *ss;
    netsnmp_pdu *response, *pdu;
    netsnmp_variable_list *vars;

    SOCK_STARTUP;

    snmp_sess_init(&session);
    session.peername = strdup(host);
    session.version = SNMP_VERSION_2c;
    session.community = (unsigned char *)community;
    session.community_len = strlen(community);

    ss = snmp_open(&session);
    if (!ss) {
        snmp_sess_perror("snmpwalk", &session);
        SOCK_CLEANUP;
        exit(1);
    }

    oid name[MAX_OID_LEN];
    size_t name_length;

    memmove(name, dnsConfigOid, sizeof(dnsConfigOid));
    name_length = sizeof(dnsConfigOid) / sizeof(oid);

    while (1) {
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);

        if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS) {
            if (response->errstat == SNMP_ERR_NOERROR) {
                for (vars = response->variables; vars; vars = vars->next_variable) {
                    if (vars->name[10] == 1) {
                        snprintf(dhcp_pools[dhcp_count], sizeof(dhcp_pools[dhcp_count]), "%s", vars->val.string);
                        dhcp_count++;
                    }

                    if (vars->name[10] == 7) {
                        snprintf(dns_server[dns_count], sizeof(dns_server[dns_count]), "%s", vars->val.string);
                        dns_count++;
                    }

                    // Check if var->name[10] becomes 8, then exit the loop
                    if (vars->name[10] == 8) {
                        goto end_of_loop;
                    }

                    // Prepare for the next instance in the walk
                    memmove(name, vars->name, vars->name_length * sizeof(oid));
                    name_length = vars->name_length;
                }
            } else {
                fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
                break;
            }
        } else {
            fprintf(stderr, "Error in request.\n");
            break;
        }
        snmp_free_pdu(response);
    }

    end_of_loop:

    printf("[");
    for (int i = 0; i < dhcp_count; ++i) {

        size_t inputLength = strlen(dns_server[i]);
        char modifiedString[inputLength + 1]; 
        strcpy(modifiedString, dns_server[i]);

        for (size_t j = 0; j < inputLength; ++j) {
         if (modifiedString[j] == ';') {
            modifiedString[j] = ',';
          }
        }
        strcpy(dns_server[i] , modifiedString);

        printf("{\"%s\": \"%s\"}", dhcp_pools[i],dns_server[i]);
        if (i < dhcp_count - 1) {
            printf(",");
        }
    }
    printf("]");


    snmp_close(ss);
    SOCK_CLEANUP;
}

void runFunction(SnmpFunction function) {
    function();
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <function>\n", argv[0]);
        return 1;
    }

    // Create a lookup table mapping function names to function pointers
    struct {
        const char *name;
        SnmpFunction function;
    } functionTable[] = {
        {"dhcpserver_dns", dhcpserver_dns},
        // Add more functions as needed
        {NULL, NULL} // End of the table
    };

    // Search for the requested function in the table
    int i = 0;
    while (functionTable[i].name != NULL) {
        if (strcmp(argv[1], functionTable[i].name) == 0) {
            // Found the function, run it
            runFunction(functionTable[i].function);
            return 0;
        }
        i++;
    }

    // If the function is not found, print an error message
    fprintf(stderr, "Unknown function: %s\n", argv[1]);
    return 1;
}
