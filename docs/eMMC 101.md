# How to write an eMMC driver for the Raspberry Pi 3B

## Initializing the eMMC

- Validate that the `sd_commands` and `sd_acommands` structures are the correct size (64 * sizeof(uint32_t))

- Power cycle the card:
  
  - Send a mailbox message to power off the card:
    
    - Contents:
      
      - `mbox[0] = 8 * sizeof(uint32_t)` - size
      
      - `mbox[1] = 0` - request code 0 = process request
      
      - `mbox[2] = MBOX_TAG_SETPOWER`
      
      - `mobx[3] = 0x8` - buffer size
        
        - size of `[4]` and `[5]`
      
      - `mbox[4]  = 0x8` - value length
        
        - size of `[5`] and `[6]`
      
      - `mbox[5] = 0x0` - the device ID is returned here
      
      - `mbox[6] = 0x2`  - power off
      
      - `mbox[7] = MBOX_TAG_LAST` 
    
    - Response (TODO: verify this! this info is very sus(TM)):
      
      - if `((mbox[6] & 0x3) != 0)`, the device did not power off successfully
      
      - if `(mbox[5]) != 0)`, we did not get a valid device id
      
      - and obviously, `mbox_send` should return 0.
    
    - Sleep for 5000 microseconds after powering off, then power on.
  
  - Power on the card. Send the same mailbox message but with`mbox[6]`  set to `0x3`

## C Definitions, etc:

```c

```
