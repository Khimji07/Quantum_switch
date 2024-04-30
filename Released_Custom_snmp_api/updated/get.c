#include <stdio.h>
#include <stdlib.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <dlfcn.h>
#include <string.h>

typedef void (*SnmpFunction)(void);
const char *host = "203.0.113.121";//203.0.113.121//172.16.100.58
const char *community = "public";

void dhcpserver_dns()
{
    
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
    if (!ss)
    {
        snmp_sess_perror("snmpwalk", &session);
        SOCK_CLEANUP;
        exit(1);
    }

    oid name[MAX_OID_LEN];
    size_t name_length;

    memmove(name, dnsConfigOid, sizeof(dnsConfigOid));
    name_length = sizeof(dnsConfigOid) / sizeof(oid);

    while (1)
    {
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);

        if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
        {
            if (response->errstat == SNMP_ERR_NOERROR)
            {
                for (vars = response->variables; vars; vars = vars->next_variable)
                {
                    if (vars->name[10] == 1)
                    {
                        snprintf(dhcp_pools[dhcp_count], sizeof(dhcp_pools[dhcp_count]), "%s", vars->val.string);
                        dhcp_count++;
                    }

                    if (vars->name[10] == 7)
                    {
                        snprintf(dns_server[dns_count], sizeof(dns_server[dns_count]), "%s", vars->val.string);
                        dns_count++;
                    }

                    // Check if var->name[10] becomes 8, then exit the loop
                    if (vars->name[10] == 8)
                    {
                        goto end_of_loop;
                    }

                    // Prepare for the next instance in the walk
                    memmove(name, vars->name, vars->name_length * sizeof(oid));
                    name_length = vars->name_length;
                }
            }
            else
            {
                fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
                break;
            }
        }
        else
        {
            fprintf(stderr, "Error in request.\n");
            break;
        }
        snmp_free_pdu(response);
    }

end_of_loop:
    // for (int i = 0; i < dhcp_count; ++i) {
    //     printf( "  {\n");
    //     printf( "    \"DHCP Pools\": \"%s\",\n", dhcp_pools[i]);

    //     printf( "    \"DNS Servers\": \"%s\"\n", dns_server[i]);

    //     printf( "  }\n");
    //     if (i < dhcp_count - 1) {
    //         printf(",\n");
    //     }
    // }
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
    printf("]\n");
    

    snmp_close(ss);
    SOCK_CLEANUP;
}
void voice_vlan()
{

    char index_name[100][255];
    char index[100][255];
    char vv_enable[100][255];
    char vv_activated[100][255];
    char vv_cosmode[100][255];
    char oui_desc[100][255];
    int index_name_count = 0, vv_enable_count = 0, oui_desc_count = 0, index_count = 0, vv_activated_count = 0, vv_cosmode_count = 0;
    oid index_OID[] = {1, 3, 6, 1, 4, 1, 89, 53, 3, 1, 1};
    oid index_name_OID[] = {1, 3, 6, 1, 4, 1, 89, 53, 3, 1, 2};
    oid vv_enable_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 12, 5, 1, 1};
    oid vv_activated_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 12, 5, 1, 4};
    oid vv_cosmode_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 12, 5, 1, 5};
    oid oui_desc_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 12, 4, 1, 2};
    oid aging_timeout_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 12, 6, 0};
    oid vv_state_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 6, 0};
    oid vv_id_OID[] = {1, 3, 6, 1, 4, 1, 89, 48, 54, 8, 0};
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
    if (!ss)
    {
        snmp_sess_perror("snmpwalk", &session);
        SOCK_CLEANUP;
        exit(1);
    }
     int aging_timeout, vv_id, vv_state;
    size_t vv_state_length = sizeof(vv_state_OID) / sizeof(oid);
    size_t aging_timeout_length = sizeof(aging_timeout_OID) / sizeof(oid);
    size_t vv_id_length = sizeof(vv_id_OID) / sizeof(oid);
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, aging_timeout_OID, aging_timeout_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
            // Print the value
            // printf("Aging TimeOut: ");
            switch (vars->type)
            {
            case ASN_INTEGER:
                // printf("%ld", *vars->val.integer);
                aging_timeout = *vars->val.integer;
                break;
            case ASN_OCTET_STR:
                // printf("%s", vars->val.string);
                aging_timeout = atoi((char*)vars->val.string);
                break;
                // Handle other data types as needed

            default:
                printf("Unsupported data type: %d", vars->type);
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error in SNMP GET request.\n");
    }
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, vv_id_OID, vv_id_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
            // Print the value
            switch (vars->type)
            {
            case ASN_INTEGER:
                vv_id = *vars->val.integer;
                break;
            case ASN_OCTET_STR:
                vv_id = atoi((char*)vars->val.string);
                break;
                // Handle other data types as needed

            default:
                printf("Unsupported data type: %d", vars->type);
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error in SNMP GET request.\n");
    }
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, vv_state_OID, vv_state_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
            // Print the value
            switch (vars->type)
            {
            case ASN_INTEGER:
                vv_state = *vars->val.integer;
                break;
            case ASN_OCTET_STR:
                vv_state = atoi((char*)vars->val.string);
                break;
                // Handle other data types as needed

            default:
                printf("Unsupported data type: %d", vars->type);
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error in SNMP GET request.\n");
    }

    // Index_Name
    oid name[MAX_OID_LEN];
    size_t name_length;
    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->non_repeaters = 0;
    pdu->max_repetitions = 52;
    name_length = sizeof(index_name_OID) / sizeof(oid);
    snmp_add_null_var(pdu, index_name_OID, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                if (vars->name_length < OID_LENGTH(index_name_OID))
                {
                    // If the received OID is not part of the requested subtree, break
                    break;
                }

                if (snmp_oid_compare(index_name_OID, OID_LENGTH(index_name_OID), vars->name, OID_LENGTH(index_name_OID)) != 0)
                {
                    // If the received OID is not part of the requested subtree, break
                    break;
                }

                // Extract the last sub-identifier to get the index
                int index_value = vars->name[vars->name_length - 1];

                // Assuming the value is already an octet string
                snprintf(index_name[index_value - 1], sizeof(index_name[index_value - 1]), "%s", vars->val.string);
            }
        }
        else
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else
    {
        fprintf(stderr, "Error in request.\n");
    }

    snmp_free_pdu(response);
    //INDEX
    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->non_repeaters = 0;
    pdu->max_repetitions = 52;
    name_length = sizeof(index_OID) / sizeof(oid);
    snmp_add_null_var(pdu, index_OID, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                if (vars->name_length < OID_LENGTH(index_name_OID))
                {
                    // If the received OID is not part of the requested subtree, break
                    break;
                }

                if (snmp_oid_compare(index_OID, OID_LENGTH(index_OID), vars->name, OID_LENGTH(index_OID)) != 0)
                {
                    // If the received OID is not part of the requested subtree, break
                    break;
                }

                // Extract the last sub-identifier to get the index
                int index_value1 = vars->name[vars->name_length - 1];

                // Assuming the value is already an octet string
                int value = *vars->val.integer;
                snprintf(index[index_value1 - 1], sizeof(index[index_value1 - 1]), "%d", value);
                index_count++;
            }
        }
        else
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else
    {
        fprintf(stderr, "Error in request.\n");
    }

    snmp_free_pdu(response);
    //Enable
    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->non_repeaters = 0;
    pdu->max_repetitions = 52;
    name_length = sizeof(vv_enable_OID) / sizeof(oid);
    snmp_add_null_var(pdu, vv_enable_OID, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                

                // Extract the last sub-identifier to get the index
                int index_value2 = vars->name[vars->name_length - 1];

                // Assuming the value is already an octet string
                int value = *vars->val.integer;
                snprintf(vv_enable[index_value2 - 1], sizeof(vv_enable[index_value2 - 1]), "%d", value);
            }
        }
        else
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else
    {
        fprintf(stderr, "Error in request.\n");
    }
    
    snmp_free_pdu(response);
    //activated
    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->non_repeaters = 0;
    pdu->max_repetitions = 52;
    name_length = sizeof(vv_activated_OID) / sizeof(oid);
    snmp_add_null_var(pdu, vv_activated_OID, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                

                // Extract the last sub-identifier to get the index
                int index_value3 = vars->name[vars->name_length - 1];

                // Assuming the value is already an octet string
                int value = *vars->val.integer;
                snprintf(vv_activated[index_value3 - 1], sizeof(vv_enable[index_value3 - 1]), "%d", value);
            }
        }
        else
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else
    {
        fprintf(stderr, "Error in request.\n");
    }

    snmp_free_pdu(response);
    //cosmode
    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->non_repeaters = 0;
    pdu->max_repetitions = 52;
    name_length = sizeof(vv_cosmode_OID) / sizeof(oid);
    snmp_add_null_var(pdu, vv_cosmode_OID, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                

                // Extract the last sub-identifier to get the index
                int index_value4 = vars->name[vars->name_length - 1];

                // Assuming the value is already an octet string
                int value = *vars->val.integer;
                snprintf(vv_cosmode[index_value4 - 1], sizeof(vv_enable[index_value4 - 1]), "%d", value);
            }
        }
        else
        {
            fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else
    {
        fprintf(stderr, "Error in request.\n");
    }

    snmp_free_pdu(response);
    
    // for(int i=0;i<52;i++){printf("%s",index[i]);}
    //Print the results
    int lastValidIndex = 0;
    for (int i = 0; i < 52; i++) {
        if (index[i][0] != '\0') {
            lastValidIndex = i;
        }
    }
    printf("[\n  ");
    printf("{\n  ");
    printf("\"Global Parameters\":{");
    printf(" \"Voice_Vlan_State\":%d,\n\t\t\t\"Voice_Vlan_Id\":%d,\n\t\t\t\"Aging_Timeout\":%d\n",vv_state,vv_id,aging_timeout);
    printf("\t\t\t}},\n");
    printf("  {\n");
    for (int i = 0; i < 52; i++)
    {
        if (index[i][0] != '\0')
        {
            printf("    \"%s\":\{\"Voice_Vlan_Enabled\":%s,\n\t\t\"Activation_Status\":%s,\n\t\t\"Cos_Mode\":%s\n\t\t}",index_name[i],vv_enable[i],vv_activated[i],vv_cosmode[i]);
        }
        if (i<lastValidIndex && index[i][0] != '\0'){
            printf(",\n");
        }
    }
    printf("  \n}\n");
    printf("]\n");
    snmp_close(ss);
    SOCK_CLEANUP;
}
void show_inventory()
{

  char sfp_pid[100][255];
  char sfp_name[100][255];
  char sfp_desc[100][255];
  char sfp_sn[100][255];

  int sfp_pid_count = 0, sfp_name_count = 0, sfp_desc_count = 0, sfp_sn_count = 0;
  oid sfp_oid[] = {1, 3, 6, 1, 4, 1, 89, 217, 1, 4, 1, 1};

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
  if (!ss)
  {
    snmp_sess_perror("snmpwalk", &session);
    SOCK_CLEANUP;
    exit(1);
  }

  oid name[MAX_OID_LEN];
  size_t name_length;

  memmove(name, sfp_oid, sizeof(sfp_oid));
  name_length = sizeof(sfp_oid) / sizeof(oid);

  while (1)
  {
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    snmp_add_null_var(pdu, name, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
      if (response->errstat == SNMP_ERR_NOERROR)
      {
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
          if (vars->name[9] == 4 && vars->name[10] == 1)
          {
            snprintf(sfp_pid[sfp_pid_count], sizeof(sfp_pid[sfp_pid_count]), "%s", vars->val.string);
            sfp_pid_count++;
          }

          if (vars->name[9] == 5 && vars->name[10] == 1)
          {
            snprintf(sfp_name[sfp_name_count], sizeof(sfp_name[sfp_name_count]), "%s", vars->val.string);
            sfp_name_count++;
          }

          if (vars->name[9] == 6 && vars->name[10] == 1)
          {
            snprintf(sfp_desc[sfp_desc_count], sizeof(sfp_desc[sfp_desc_count]), "%s", vars->val.string);
            sfp_desc_count++;
          }

          if (vars->name[9] == 7 && vars->name[10] == 1)
          {
            snprintf(sfp_sn[sfp_sn_count], sizeof(sfp_sn[sfp_sn_count]), "%s", vars->val.string);
            sfp_sn_count++;
          }

          if (vars->name[9] == 8)
          {
            goto end_of_loop;
          }

          // Prepare for the next instance in the walk
          memmove(name, vars->name, vars->name_length * sizeof(oid));
          name_length = vars->name_length;
        }
      }
      else
      {
        fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        break;
      }
    }
    else
    {
      fprintf(stderr, "Error in request.\n");
      break;
    }
    snmp_free_pdu(response);
  }

end_of_loop:

  printf("[\n");
for (int i = 0; i < sfp_pid_count; i++) {
    printf("  {\n");
    printf("    \"SFP_PID\": \"%s\",\n", sfp_pid[i]);
    printf("    \"SFP_Port_Name\": \"%s\",\n", sfp_name[i]);
    printf("    \"SFP_Desc\": \"%s\",\n", sfp_desc[i]);
    printf("    \"SFP_SerialNo\": \"%s\"\n", sfp_sn[i]);
    if (i < sfp_pid_count - 1) {
        printf("  },\n");
    } else {
        printf("  }\n");
    }
}
printf("]\n");

  snmp_close(ss);
  SOCK_CLEANUP;
}
void trans_info()
{
  clock_t start_time, end_time;
    double elapsed_time; 
    start_time = clock();
  char trans_temp[100][255];
  int trans_temp_int[100]; // Array to store integer temperature values
  char trans_volt[100][255];
  int trans_volt_int[100];
  char trans_current[100][255];
  int trans_current_int[100];
  char trans_ipower[100][255];
  int trans_ipower_int[100];
  char trans_opower[100][255];
  int trans_opower_int[100];
  char trans_LOS[100][255];
  int trans_LOS_int[100];
  int trans_temp_count = 0, trans_volt_count = 0, trans_current_count = 0, trans_ipower_count = 0, trans_opower_count = 0, trans_LOS_count = 0;
  oid trans_oid[] = {1, 3, 6, 1, 4, 1, 89, 90, 1, 2, 1, 3, 1, 1};
  

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
  if (!ss)
  {
    snmp_sess_perror("snmpwalk", &session);
    SOCK_CLEANUP;
    exit(1);
  }
  oid name[MAX_OID_LEN];
  size_t name_length;
  memmove(name, trans_oid, sizeof(trans_oid));
  name_length = sizeof(trans_oid) / sizeof(oid);

  while (1)
  {
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    snmp_add_null_var(pdu, name, name_length);

    if (snmp_synch_response(ss, pdu, &response) == STAT_SUCCESS)
    {
      if (response->errstat == SNMP_ERR_NOERROR)
      {
        for (vars = response->variables; vars; vars = vars->next_variable)
        {
          // Print the OID values

          if (vars->name[13] == 5)
          {
            if (vars->val.integer != NULL)
            {
              trans_temp_int[trans_temp_count] = *vars->val.integer;
              // Convert integer temperature to string
              snprintf(trans_temp[trans_temp_count], sizeof(trans_temp[trans_temp_count]), "%d", trans_temp_int[trans_temp_count]);
              trans_temp_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("Temperature value is NULL.\n");
            }
          }
          if (vars->name[13] == 6)
          {
            if (vars->val.integer != NULL)
            {
              trans_volt_int[trans_volt_count] = *vars->val.integer;
              // Convert integer current to string
              snprintf(trans_volt[trans_volt_count], sizeof(trans_volt[trans_volt_count]), "%d", trans_volt_int[trans_volt_count]);
              trans_volt_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("Volt value is NULL.\n");
            }
          }
          if (vars->name[13] == 7)
          {
            if (vars->val.integer != NULL)
            {
              trans_current_int[trans_current_count] = *vars->val.integer;
              // Convert integer current to string
              snprintf(trans_current[trans_current_count], sizeof(trans_current[trans_current_count]), "%d", trans_current_int[trans_current_count]);
              trans_current_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("Current value is NULL.\n");
            }
          }
          if (vars->name[13] == 8)
          {
            if (vars->val.integer != NULL)
            {
              trans_opower_int[trans_opower_count] = *vars->val.integer;
              // Convert integer current to string
              snprintf(trans_opower[trans_opower_count], sizeof(trans_opower[trans_opower_count]), "%d", trans_opower_int[trans_opower_count]);
              trans_opower_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("Output power value is NULL.\n");
            }
          }
          if (vars->name[13] == 9)
          {
            if (vars->val.integer != NULL)
            {
              trans_ipower_int[trans_ipower_count] = *vars->val.integer;
              // Convert integer current to string
              snprintf(trans_ipower[trans_ipower_count], sizeof(trans_ipower[trans_ipower_count]), "%d", trans_ipower_int[trans_ipower_count]);
              trans_ipower_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("Input power value is NULL.\n");
            }
          }

          if (vars->name[13] == 11)
          {
            if (vars->val.integer != NULL)
            {
              trans_LOS_int[trans_LOS_count] = *vars->val.integer;
              // Convert integer current to string
              snprintf(trans_LOS[trans_LOS_count], sizeof(trans_LOS[trans_LOS_count]), "%d", trans_LOS_int[trans_LOS_count]);
              trans_LOS_count++;
            }
            else
            {
              // Handle the case where the value is NULL (optional)
              printf("LOS value is NULL.\n");
            }
          }
          if(vars->name[13] == 12){
            goto x;
          }
          // Check if var->name[10] becomes 8, then exit the loop
          if (vars->name[11] == 4)
          {
            goto end_of_loop;
          }
          x:
          // Prepare for the next instance in the walk
          memmove(name, vars->name, vars->name_length * sizeof(oid));
          name_length = vars->name_length;
        }
      }
      else
      {
        fprintf(stderr, "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
        break;
      }
    }
    else
    {
      fprintf(stderr, "Error in request.\n");
      break;
    }
    snmp_free_pdu(response);
  }
   

end_of_loop:
  int count = 0;
  printf("{\n");
    for (int i = 0; i < trans_temp_count; i++) {
        printf("    \"tel1/0/%d\": {\n", count + 1);
        printf("        \"Temperature\": %s,\n", trans_temp[i]);
        printf("        \"Voltage\": %s,\n", trans_volt[i]);
        printf("        \"Current\": %s,\n", trans_current[i]);
        printf("        \"Output Power\": %s,\n", trans_opower[i]);
        printf("        \"Input Power\": %s,\n", trans_ipower[i]);
        printf("        \"LOS\": %s\n", trans_LOS[i]);
        printf("    }");
        if (i < trans_temp_count - 1) {
            printf(",");
        }
        printf("\n");
        count++;
    }
    printf("}\n");
  snmp_close(ss);
  SOCK_CLEANUP;
}

void runFunction(SnmpFunction function)
{
    function();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <function>\n", argv[0]);
        return 1;
    }

    // Create a lookup table mapping function names to function pointers
    struct
    {
        const char *name;
        SnmpFunction function;
    } functionTable[] = {
        {"dhcpserver_dns", dhcpserver_dns},
        {"voice_vlan", voice_vlan},
        {"show_inventory", show_inventory},
        {"trans_info", trans_info},
        // Add more functions as needed
        {NULL, NULL} // End of the table
    };

    // Search for the requested function in the table
    int i = 0;
    while (functionTable[i].name != NULL)
    {
        if (strcmp(argv[1], functionTable[i].name) == 0)
        {
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
