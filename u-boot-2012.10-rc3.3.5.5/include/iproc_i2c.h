/*
 * $Copyright Open Broadcom Corporation$
 */

#ifndef __IPROC_SMBUS_H__
#define __IPROC_SMBUS_H__

#define IPROC_I2C_INVALID_ADDR 0xFF

#define IPROC_SMB_MAX_RETRIES   35

#define I2C_SMBUS_BLOCK_MAX     32

#define GETREGFLDVAL(regval, mask, startbit) (((regval) & (mask)) >> (startbit))

#define SETREGFLDVAL(regval, fldval, mask, startbit) regval = \
                                                      (regval & ~(mask)) | \
                                                      ((fldval) << (startbit))

/* Enum to specify clock speed. The user will provide it during initialization.
 * If needed, it can be changed dynamically
 */
typedef enum iproc_smb_clk_freq {
    I2C_SPEED_100KHz = 0,
    I2C_SPEED_400KHz = 1,
    I2C_SPEED_INVALID = 255
} smb_clk_freq_t;

/* This enum will be used to notify the user of status of a data transfer
 * request 
 */
typedef enum iproc_smb_error_code {
    I2C_NO_ERR = 0,
    I2C_TIMEOUT_ERR = 1,
    I2C_INVALID_PARAM_ERR = 2, /* Invalid parameter(s) passed to the driver */
    I2C_OPER_IN_PROGRESS = 3, /* The driver API was called before the present
                                 transfer was completed */
    I2C_OPER_ABORT_ERR = 4, /* Transfer aborted unexpectedly, for example a NACK
                               received, before last byte was read/written */
    I2C_FUNC_NOT_SUPPORTED = 5, /* Feature or function not supported 
	                               (e.g., 10-bit addresses, or clock speeds
                                   other than 100KHz, 400KHz) */
} iproc_smb_error_code_t;


/* Structure used to pass information to read/write functions. */
struct iproc_xact_info {
    unsigned char command; /* Passed by caller to send SMBus command code */
    unsigned char *data; /* actual data pased by the caller */
    unsigned int size; /* Size of data buffer passed */
    unsigned short flags; /* Sent by caller specifying PEC, 10-bit addresses */
    unsigned char smb_proto; /* SMBus protocol to use to perform transaction */
    unsigned int cmd_valid; /* true if command field below is valid. Otherwise, false */
};

#endif /* __IPROC_SMBUS_H__ */
