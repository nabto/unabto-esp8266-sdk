#include "fp_acl_eeprom.h"

#include <unabto/unabto_util.h>
#include <EEPROM.h>

size_t eeprom_size = 0;

fp_acl_db_status fp_acl_eeprom_load(struct fp_mem_state* acl)
{
    size_t addr = 0;
    uint8_t buffer[128];

    // load version
    for(size_t i = 0; i < 4; ++i) {
        buffer[i] = EEPROM.read(addr++);
    }

    uint32_t version;
    READ_U32(version, buffer);
    if (version != FP_ACL_FILE_VERSION) {
        // There is no saved valid acl database, consider it as a completely
        // normal bootstrap scenario.
        return FP_ACL_DB_OK;
    }

    memset(acl, 0, sizeof(struct fp_mem_state));

    // load system settings
    for(size_t i = 0; i < 16; ++i) {
        buffer[i] = EEPROM.read(addr++);
    }

    uint8_t* ptr = buffer;

    uint32_t numUsers;

    READ_FORWARD_U32(acl->settings.systemPermissions, ptr);
    READ_FORWARD_U32(acl->settings.defaultUserPermissions, ptr);
    READ_FORWARD_U32(acl->settings.firstUserPermissions, ptr);
    READ_FORWARD_U32(numUsers, ptr);

    uint32_t i;
    enum {
        USER_RECORD_SIZE = FP_ACL_FP_LENGTH + FP_ACL_FILE_USERNAME_LENGTH + 4
    };
    for(i = 0; i < numUsers && i < FP_MEM_ACL_ENTRIES; i++) {
        for(size_t j = 0; j < USER_RECORD_SIZE; ++j) {
            buffer[j] = EEPROM.read(addr++);
        }
        memcpy(acl->users[i].fp, buffer, FP_ACL_FP_LENGTH);
        memcpy(acl->users[i].name, buffer + FP_ACL_FP_LENGTH, FP_ACL_FILE_USERNAME_LENGTH); // guaranteed by compile time check
        READ_U32(acl->users[i].permissions, buffer + FP_ACL_FP_LENGTH + FP_ACL_FILE_USERNAME_LENGTH);
    }

    if(addr > eeprom_size){
        return FP_ACL_DB_LOAD_FAILED;
    }

    return FP_ACL_DB_OK;
}

fp_acl_db_status fp_acl_eeprom_save(struct fp_mem_state* acl)
{
    uint8_t buffer[128];
    uint8_t* ptr = buffer;
    uint32_t users = 0;
    int i;
    for (i = 0; i < FP_MEM_ACL_ENTRIES; i++) {
        struct fp_acl_user* it = &acl->users[i];
        if (!fp_mem_is_slot_free(it)) {
            users++;
        }
    }

    WRITE_FORWARD_U32(ptr, FP_ACL_FILE_VERSION);
    WRITE_FORWARD_U32(ptr, acl->settings.systemPermissions);
    WRITE_FORWARD_U32(ptr, acl->settings.defaultUserPermissions);
    WRITE_FORWARD_U32(ptr, acl->settings.firstUserPermissions);
    WRITE_FORWARD_U32(ptr, users);

    size_t addr = 0;
    for(size_t i = 0; i < 20; ++i) {
        EEPROM.write(addr++, buffer[i]);
    }

    // write user records

    for (i = 0; i < users; i++) {
        struct fp_acl_user* it = &acl->users[i];
        if (!fp_mem_is_slot_free(it)) {
            memcpy(buffer, it->fp, 16);
            memcpy(buffer+16, it->name, 64);
            WRITE_U32(buffer + 16 + 64, it->permissions);

            for(size_t i = 0; i < 84; ++i) {
                EEPROM.write(addr++, buffer[i]);
            }
        }
    }

    EEPROM.commit();

    if(addr > eeprom_size){
        return FP_ACL_DB_SAVE_FAILED;
    }

    return FP_ACL_DB_OK;
}

fp_acl_db_status fp_acl_eeprom_reset()
{
    // Reset by writing invalid version. This will be considered as bootstrap
    // scenario by next fp_acl_eeprom_load().
    size_t addr = 0;
    for(size_t i = 0; i < 4; ++i) {
        EEPROM.write(addr++, 0xff);
    }
    return FP_ACL_DB_OK;
}

fp_acl_db_status fp_acl_eeprom_init(size_t size, struct fp_mem_persistence* per)
{
    EEPROM.begin(size);
    eeprom_size = size;

    per->load = &fp_acl_eeprom_load;
    per->save = &fp_acl_eeprom_save;

    return FP_ACL_DB_OK;
}