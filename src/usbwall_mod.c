/**
 * \file usbwall_mod.c
 * \brief module for USB mass-storage device filtering
 * \author David FERNANDES
 * \version 0.1
 * \date 19 december 2011
 *
 * File usbwall_mod.c for project usbwall
 * 
 * Made by David FERNANDES
 * Login   <dfernand@ece.fr>
 *
 * Started on  Mon Dec  19 9:17:29 2011 David FERNANDES
 *
 * Copyright (C) 2012 - David FERNANDES
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* Headers */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include "storage.h"
#include "trace.h"
#include "mass_storage_info.h"

/* Module informations */
MODULE_AUTHOR ("David FERNANDES");
MODULE_DESCRIPTION ("Module for USB mass-storage device filtering");
MODULE_LICENSE ("GPL");

/* Storage class code : Mass storage */
#define USB_CLASS_MASS_STORAGE 8

/**
 * \struct usb_device_id usbwall_id_table []
 *
 * Devices supported by the module : All mass storage
 */
static struct usb_device_id usbwall_id_table[] = {

  /* Typically, flash devices : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_RBC, USB_PR_DEVICE)},

  /* CD-ROM : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8020, USB_PR_DEVICE)},

  /* QIC-157 Tapes  : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_QIC, USB_PR_DEVICE)},

  /* Floppy : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_UFI, USB_PR_DEVICE)},

  /* Removable media : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_8070, USB_PR_DEVICE)},

  /* Transparent media : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_SCSI, USB_PR_DEVICE)},

  /* Password-protected  : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_LOCKABLE, USB_PR_DEVICE)},

  /*   ISD200 ATA : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_ISD200, USB_PR_DEVICE)},

  /* Cypress ATACB : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_CYP_ATACB, USB_PR_DEVICE)},

  /* Use device's value : all storage protocol */
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_CBI)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_CB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_BULK)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_UAS)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_USBAT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_EUSB_SDDR09)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_SDDR55)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_DPCM_USB)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_FREECOM)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_DATAFAB)},
  {USB_INTERFACE_INFO  (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_JUMPSHOT)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_ALAUDA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_KARMA)},
  {USB_INTERFACE_INFO (USB_CLASS_MASS_STORAGE, USB_SC_DEVICE, USB_PR_DEVICE)},

  {}
};
MODULE_DEVICE_TABLE (usb, usbwall_id_table);

/* kernel's representation of a USB device */
static struct usb_device *dev;

/* my variables */
static char serialnumber[32] = "";
static char my_idSerialNumber[32] = "001D92DC4AF0C95163A2092C";
static struct mass_storage_info my_pen_drive;
static int i, usbwall_register, chardev_register, cmpserialnumber;

/** 
 * \fn usbwall_probe
 * \param *intf usb_interface
 * \param *devid usb_device_id
 * \return -ENOTSUP if the device is on the white liste else 0 
 *
 * Function called by the kernel when a device is detected
 */
static int usbwall_probe (struct usb_interface *intf, const struct usb_device_id *devid)
{ 
  DBG_TRACE ("entering in the function probe");

  /* Temporary white liste : my pen drive initialization */
  my_pen_drive.idVendor = 0x0930;
  my_pen_drive.idProduct =  0x6544;
  i = 0;
  while(my_idSerialNumber[i] != '\0')
  {
    my_pen_drive.idSerialNumber[i] = my_idSerialNumber[i];
    i++;
  }
  /*******************************************************/
  
  dev = interface_to_usbdev (intf);
  usb_string (dev, dev->descriptor.iSerialNumber, serialnumber, 32);

  cmpserialnumber = strcmp (serialnumber, my_pen_drive.idSerialNumber);

  /* Research if the device is on the white list */
  /* If the device is on the white liste : the module is released */
  if (dev->descriptor.idProduct == my_pen_drive.idProduct &&
      dev->descriptor.idVendor == my_pen_drive.idVendor &&
      cmpserialnumber == 0)
  {
    DBG_TRACE ("the device is on the white liste");
    return -EMEDIUMTYPE;
  }
  /* Else : creation a fake device */
  DBG_TRACE ("the device is not on the white liste");
  DBG_TRACE ("its idVendor is %x", dev->descriptor.idVendor);
  DBG_TRACE ("its idProduct is %x", dev->descriptor.idProduct);
  DBG_TRACE ("its SerialNumber is %s", serialnumber);

  return 0;
}

/** 
 * \fn usbwall_disconnect
 * \param  struct usb_interface *intf
 *
 * Function called when a device is desconnected
 */
static void usbwall_disconnect (struct usb_interface *intf)
{
  DBG_TRACE ("device disconnected");
}

/** 
 * \struct struct usb_driver usbwall_driver
 *
 * Identifies USB interface driver to usbcore
 */
static struct usb_driver usbwall_driver = {
  .name = "usbwall",
  .probe = usbwall_probe,
  .disconnect = usbwall_disconnect,
  .id_table = usbwall_id_table,
};

/* char device */

/** 
 * \fn chardev_open
 * \param struct inode *inode
 * \param struct file *file
 * \return 0 if openning success
 *
 * Open charactere device
 */
int chardev_open(struct inode *inode, struct file *file)
{
  DBG_TRACE("chardev opened");
  return 0; 
}

/** 
 * \fn chardev_release
 * \param struct inode *inode
 * \param struct file *file
 * \return 0 if closing success
 *
 * Close charactere device
 */
int chardev_release(struct inode *inode, struct file *file)
{
  DBG_TRACE("chardev release"); 
  return 0;
}

/** 
 * \fn chardev_ioctl
 * \param struct inode *inode
 * \param struct file *file
 * \param unsigned int command
 * \param unsigned long argument
 * \return 0 if closing success
 *
 * Close charactere device
 */
int chardev_ioctl(struct inode * inode, struct file * file, unsigned int command, unsigned long argument)
{
  switch(command){
    case IO_SET_PENDRIVE:
       DBG_TRACE("Argument = %lu", argument);
       break;
    default : return -ENOTTY;
  }
  return 0;
}

/**
 * \struct file_operations usbwall_chardev
 *
 * Identifies Char Device to usbcore
 */
static struct file_operations usbwall_chardev = {
  .ioctl = chardev_ioctl,
  .open = chardev_open,
  .release = chardev_release,
};

/** 
 * \fn __init usbwall_init
 * \return usbwall_register; O if register success, else error number (register failed). 
 *
 * Loading the module : register USB driver and charactere device
 */
static int __init usbwall_init (void)
{
  /* USB driver register*/
  usbwall_register = 0;
  usbwall_register = usb_register (&usbwall_driver);
  if (usbwall_register)
  {
    DBG_TRACE ("Registering usb driver failed, error : %d", usbwall_register);
  }

  /* Charactere device register*/
  chardev_register = register_chrdev(0, "usbwall_chardev", &usbwall_chardev);
  DBG_TRACE ("major number assigned to charactere device is %d", chardev_register);
  
  DBG_TRACE ("module loaded");
  return usbwall_register;
}

/** 
 * \fn __exit usbwall_exit (void)
 *
 * Unloading the module : unregister USB driver and charactere device
 */
static void __exit usbwall_exit (void)
{

  /* USB driver unregister*/
  usb_deregister (&usbwall_driver);
  
  /* Charactere device unregister*/
  unregister_chrdev(chardev_register, "usbwall_chardev");

  DBG_TRACE ("module unloaded");
}

module_init (usbwall_init);
module_exit (usbwall_exit);
