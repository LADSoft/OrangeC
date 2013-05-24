/* 
   winiomac.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define DEVICE_TYPE_FROM_CTL_CODE(ctrlCode)     (((DWORD)(ctrlCode & 0xffff0000)) >> 16)

#define IsRecognizedPartition( PartitionType ) (       \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_FAT_12)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_FAT_16)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_IFS)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_HUGE)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_FAT32)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_FAT32_XINT13)) ||  \
     ((PartitionType & PARTITION_NTFT) && ((PartitionType & ~0xC0) == PARTITION_XINT13)) ||  \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_FAT_12) ||  \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_FAT_16) ||  \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_IFS)    ||  \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_HUGE)    ||  \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_FAT32)  || \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_FAT32_XINT13) || \
     ((PartitionType & ~PARTITION_NTFT) == PARTITION_XINT13) )


#define IsContainerPartition( PartitionType ) \
    ((PartitionType == PARTITION_EXTENDED) || (PartitionType == PARTITION_XINT13_EXTENDED))
