#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <gio/gio.h>
#include <dbus/dbus.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "include/bluetooth.h"
#include "include/stack.h"

#define OBJECT_PATH_KEY "OBJECT_PATH"
#define HCI_EVENT_HDR_SIZE 2
#define HCI_LE_META_EVENT 0x3E
#define HCI_EV_LE_ADVERTISING_REPORT 0x02

static GMainLoop *main_loop = NULL;

typedef struct key_value_pair
{
    char *key;
    void *value;
} key_value_pair;

key_value_pair *create_pair(char *key, void *value)
{
    key_value_pair *pair = (key_value_pair *)malloc(sizeof(key_value_pair));
    pair->key = key;
    pair->value = value;

    return pair;
}

bool is_dbus_running()
{
    printf("is_dbus_running");
    fflush(stdout);
    GError *error = NULL;
    int status = system("systemctl is-active --quiet dbus");
    printf("is_dbus_running %b", WIFEXITED(status) && WEXITSTATUS(status) == 0);
    fflush(stdout);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool can_access_dbus()
{
    printf("gcan_access_dbus");
    fflush(stdout);
    GError *error = NULL;
    GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error)
    {
        g_error_free(error);
        return false;
    }
    g_object_unref(conn);
    printf("can_access_dbus ok");
    fflush(stdout);
    return true;
}

static void print_bluetooth_info()
{
    int dev_id = hci_get_route(NULL);
    if (dev_id < 0)
    {
        perror("No Bluetooth Adapter Found");
        exit(1);
    }

    int sock = hci_open_dev(dev_id);
    if (sock < 0)
    {
        perror("Failed to open device");
        exit(1);
    }

    // Get the device name
    char name[248];
    if (hci_read_local_name(sock, sizeof(name), name, 0) < 0)
    {
        strcpy(name, "[unknown]");
    }
    printf("Local Device Name: %s\n", name);

    // Get the device address
    bdaddr_t bdaddr;
    char addr[19];
    if (hci_devba(dev_id, &bdaddr) < 0)
    {
        strcpy(addr, "[unknown]");
    }
    else
    {
        ba2str(&bdaddr, addr);
    }
    printf("Local Device Address: %s\n", addr);

    // Get the device class
    uint8_t dev_class[3];
    struct hci_dev_info di;
    if (hci_devinfo(dev_id, &di) < 0)
    {
        perror("Can't get device info");
        exit(1);
    }
    // Get the device class
    printf("Name %s Addr: %s\n", di.name, addr);
    printf("Features: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%02x ", di.features[i]);
    }
    printf("\n");
    close(sock);
}

// Function to check for DBus errors
void check_and_abort(DBusError *error)
{
    if (dbus_error_is_set(error))
    {
        fprintf(stderr, "Error: %s\n", error->message);
        dbus_error_free(error);
        exit(EXIT_FAILURE);
    }
}

stack *get_dictionary_values(DBusMessageIter *iter)
{
    stack *result = initialize();
    flatten_dict_values(iter, result);

    return result;
}

void *get_value(DBusMessageIter *iter, stack *result)
{
    switch (dbus_message_iter_get_arg_type(iter))
    {
    case DBUS_TYPE_VARIANT:
    {
        printf("get_value variant. \n");
        DBusMessageIter variantIter;
        dbus_message_iter_recurse(iter, &variantIter);
        int val;
        return get_value(&variantIter, &val);
    }
    case DBUS_TYPE_OBJECT_PATH:
    {
        printf("get_value object path. \n");

        char *key;
        dbus_message_iter_get_basic(iter, &key);

        printf("object path value. %s\n", key);
        break;
    }
    case DBUS_TYPE_STRING:
    {
        printf("get_value string. \n");

        char *key;
        dbus_message_iter_get_basic(iter, &key);

        printf("string value. %s\n", key);

        return key;
    }
    case DBUS_TYPE_ARRAY:
    case DBUS_TYPE_DICT_ENTRY:
    {
        flatten_dict_values(iter, result);
        return NULL;
    }
    }

    return NULL;
}

void flatten_dict_values(DBusMessageIter *iter, stack *result)
{
    printf("flatten_dict_values %i\n", dbus_message_iter_get_arg_type(iter));

    switch (dbus_message_iter_get_arg_type(iter))
    {
    case DBUS_TYPE_OBJECT_PATH:
    {
        printf("go object path\n");

        char *rawValue;
        dbus_message_iter_get_basic(iter, &rawValue);

        char *value = (char *)malloc(strlen(rawValue) + 1);

        if (value != NULL)
        {
            strncpy(value, rawValue, strlen(rawValue));
        }
        else
        {
            fprintf(stderr, "failed to allocate memory for object path.");
        }
        key_value_pair *pair = create_pair(OBJECT_PATH_KEY, value);
        push(result, pair);

        printf("[object path = %s]\n", value);

        dbus_message_iter_next(iter);
        flatten_dict_values(iter, result);
        break;
    }
    case DBUS_TYPE_VARIANT:
    case DBUS_TYPE_STRING:
    {
        printf("get_simple_value string\n");
        // push(result, get_value(iter, result));

        dbus_message_iter_next(iter);
        break;
    }
    case DBUS_TYPE_ARRAY:
    {
        printf("get_simple_value array start.  %i \n", iter);

        DBusMessageIter arrayIter;
        dbus_message_iter_recurse(iter, &arrayIter);

        flatten_dict_values(&arrayIter, result);

        printf("get_simple_value array end. %i \n", iter);

        dbus_message_iter_next(iter);
        break;
    }
    case DBUS_TYPE_DICT_ENTRY:
    {
        printf("get_simple_value dictionary start. %i \n", iter);

        while (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_DICT_ENTRY)
        {
            DBusMessageIter entryIter;
            dbus_message_iter_recurse(iter, &entryIter);

            char *key;
            dbus_message_iter_get_basic(&entryIter, &key);

            DBusMessageIter value_iter;
            dbus_message_iter_next(&entryIter);
            dbus_message_iter_recurse(&entryIter, &value_iter);

            int valueType;
            void *value = get_value(&entryIter, result);

            if (value != NULL)
            {
                key_value_pair *resultItem = malloc(sizeof(key_value_pair));
                resultItem->key = key;
                resultItem->value = value;

                push(result, resultItem);
            }
            dbus_message_iter_next(iter);
        }

        printf("get_simple_value dictionary end. %i \n", iter);
        break;
    }
    case DBUS_TYPE_INVALID:
    {
        return;
    }
    default:
    {
        printf("unhandler type %i \n", dbus_message_iter_get_arg_type(iter));
        break;
    }
    }
}

void connect_device(DBusConnection *connection, const char *deviceObjectPath, DBusError *error)
{
    printf("connect %s", deviceObjectPath);

    DBusMessage *message = dbus_message_new_method_call(
        "org.bluez",
        deviceObjectPath,
        "org.bluez.Device1",
        "Connect");

    if (message == NULL)
    {
        fprintf(stderr, "connect message NULL");
        exit(EXIT_FAILURE);
    }

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(connection, message, -1, error);

    printf("Connected to %s", deviceObjectPath);

    if (reply != NULL)
    {
        dbus_message_unref(reply);
    }

    dbus_message_unref(message);
}

bool signal_handler(DBusConnection *connection, DBusMessage *message)
{
    printf("signal_handler %i", message);
    fflush(stdout);
    if (dbus_message_is_signal(message, "org.freedesktop.DBus.ObjectManager", "InterfacesAdded"))
    {
        printf("InterfacesAdded Signal Received\n");
        fflush(stdout);
        DBusMessageIter iter;
        if (dbus_message_iter_init(message, &iter) == FALSE)
        {
            printf("failed to iter_init\n");
            fflush(stdout);
        }
        else
        {
            printf("interface added");
            fflush(stdout);

            DBusMessageIter args;
            if (dbus_message_iter_init(message, &args) == FALSE)
            {
                printf("failed to init iterator");
                fflush(stdout);
                return;
            }
            printf("init iterator.\n");
            fflush(stdout);

            stack *result = get_dictionary_values(&args);

            printf("got stack %i.\n", result->length);
            while (!is_empty(result))
            {
                printf("pop\n");
                pop_result popResult = pop(result);

                if (popResult.error)
                {
                    printf("error pop %s\n", popResult.error);
                }
                else
                {
                    printf("convert value\n");
                    key_value_pair *entry = (key_value_pair *)popResult.data;

                    printf("[key: %s|v: %s]\n", entry->key, entry->value);
                    if (strcmp(OBJECT_PATH_KEY, entry->key) == 0)
                    {
                        printf("Found object path %s. Connecting...\n", entry->value);

                        if (strstr(entry->value, "14_CB_65_88_01_FF") == NULL)
                        {
                            printf("skip...\n");
                            continue;
                        }

                        DBusError connectError;
                        dbus_bool_t errorSet;
                        do
                        {
                            dbus_error_init(&connectError);
                            connect_device(connection, entry->value, &connectError);
                            errorSet = dbus_error_is_set(&connectError);
                            if (errorSet)
                            {
                                errorSet = TRUE;
                                fprintf(stderr, "connection error: %s\n", connectError.message);
                                dbus_error_free(&connectError);
                            }
                            else
                            {
                                errorSet = FALSE;
                                printf("connected. %s\n", entry->value);
                                dbus_error_free(&connectError);
                                return TRUE;
                            }
                            sleep(1);
                        } while (errorSet);
                    }

                    if (entry != NULL)
                    {
                        printf("free()\n");
                        free(entry);
                    }
                }
            }

            deinitialize(result);
        }
    }
    else if (dbus_message_is_signal(message, "org.freedesktop.DBus.Properties", "PropertiesChanged"))
    {
    }

    return FALSE;
}

static void start_scan(GDBusProxy *adapter)
{
    print_bluetooth_info();

    DBusError error;
    dbus_error_init(&error);

    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

    check_and_abort(&error);

    char *rule = "type='signal',sender='org.bluez'";
    dbus_bus_add_match(connection, rule, &error);
    check_and_abort(&error);
    bool connected = FALSE;
    do
    {
        printf("Start Discovery\n");
        DBusMessage *message = dbus_message_new_method_call(
            "org.bluez",
            "/org/bluez/hci0",
            "org.bluez.Adapter1",
            "StartDiscovery");

        check_and_abort(&error);

        printf("ok\n");

        printf("Start Discovery reply and block\n");
        DBusMessage *reply = dbus_connection_send_with_reply_and_block(
            connection,
            message,
            5000,
            &error);
        check_and_abort(&error);

        printf("ok\n");
        dbus_message_unref(message);

        if (reply != NULL)
        {
            dbus_message_unref(reply);
        }

        printf("Discovery...\n");
        fflush(stdout);
        int tries = 0;
        while (++tries < 10)
        {
            // Dispatch messages
            while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS)
            {
            }

            dbus_connection_read_write(connection, 1000);
            DBusMessage *signalMessage = dbus_connection_pop_message(connection);

            if (signalMessage == NULL)
            {
                printf("no message\n");
                sleep(1);
                continue;
            }

            if (signal_handler(connection, signalMessage))
            {
                connected = TRUE;
            }

            dbus_message_unref(signalMessage);
            sleep(2);
        }

        // Stop discovery
        message = dbus_message_new_method_call(
            "org.bluez",
            "/org/bluez/hci0",
            "org.bluez.Adapter1",
            "StopDiscovery");

        check_and_abort(&error);

        reply = dbus_connection_send_with_reply_and_block(connection, message, -1, &error);
        check_and_abort(&error);

        dbus_message_unref(message);
        if (reply != NULL)
        {
            dbus_message_unref(reply);
        }

        sleep(5);
    } while (!connected);
}

bool connectBluetoothDevice(char *deviceMacAddress)
{

    start_scan(NULL);

    //     GDBusProxy* adapter = get_adapter();
    //     if (!adapter) {
    //         return FALSE; // or handle error appropriately
    //     }
    //     fflush(stdout);
    //     listen_for_devices();
    // fflush(stdout);
    //     start_scan(adapter);
    // fflush(stdout);
    //     g_object_unref(adapter); // clean up
    return TRUE;
}

bool findAndConnectBluetooth()
{
    return connectBluetoothDevice(NULL);
}