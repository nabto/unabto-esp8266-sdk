#ifndef _FP_ACL_EEPROM_H_
#define _FP_ACL_EEPROM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <modules/fingerprint_acl/fp_acl.h>
#include <modules/fingerprint_acl/fp_acl_memory.h>
#ifdef __cplusplus
} // extern "C"
#endif

/**
 * The fp acl eeprom saves the fingerprint config in the following format
 *
 * Version 2 of the file format is as follows:
 *
 * uint32_t version
 * settings { uint32_t systemPermissions, uint32_t defaultUserPermissions, uint32_t firstUserPermissions}
 * number of users: uint32_t,
 * [users] user = {fingerprint: uint8_t[16], name: char[64], permissions: uint32_t}
 *
 * saving rewrites the whole eeprom.
 *
 * init loads the file the first time it
 *
 * in version 1 the name is always 64 bytes long
 */

#define FP_ACL_FILE_USERNAME_LENGTH 64

#define FP_ACL_FILE_VERSION 2

#if FP_ACL_USERNAME_MAX_LENGTH != FP_ACL_FILE_USERNAME_LENGTH
#error incompatible user name length with current acl file format
#endif


fp_acl_db_status fp_acl_eeprom_save(struct fp_mem_state* acl);

fp_acl_db_status fp_acl_eeprom_load(struct fp_mem_state* acl);

/**
 * Inititalize the eeprom persisting backend.
 */
fp_acl_db_status fp_acl_eeprom_init(size_t size, struct fp_mem_persistence* per);


#endif
