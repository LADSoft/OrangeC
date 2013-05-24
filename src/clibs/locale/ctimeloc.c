/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <limits.h>
#include <stddef.h>
#include <ctype.h>
#include <locale.h>
#include "_locale.h"
#include "llp.h"
static char C_qualified_name[] = "C";

ABBREVIATED_TIME_DATA C_time_data= {
    &C_qualified_name,
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
    "am",
    "pm",
    "%a %b %d %Y",
    "%a%b%d",
    "%I:%M:%S",
    "%I:%M:%S %p",

    {
    "0th\000"
    "1st\000"
    "2nd\000"
    "3rd\000"
    "4th\000"
    "5th\000"
    "6th\000"
    "7th\000"
    "8th\000"
    "9th\000"
    "10th\000"
    "11th\000"
    "12th\000"
    "13th\000"
    "14th\000"
    "15th\000"
    "16th\000"
    "17th\000"
    "18th\000"
    "19th\000"
    "20th\000"
    "21st\000"
    "22nd\000"
    "23rd\000"
    "24th\000"
    "25th\000"
    "26th\000"
    "27th\000"
    "28th\000"
    "29th\000"
    "30th\000"
    "31st\000"
    "32nd\000"
    "33rd\000"
    "34th\000"
    "35th\000"
    "36th\000"
    "37th\000"
    "38th\000"
    "39th\000"
    "40th\000"
    "41st\000"
    "42nd\000"
    "43rd\000"
    "44th\000"
    "45th\000"
    "46th\000"
    "47th\000"
    "48th\000"
    "49th\000"
    "50th\000"
    "51st\000"
    "52nd\000"
    "53rd\000"
    "54th\000"
    "55th\000"
    "56th\000"
    "57th\000"
    "58th\000"
    "59th\000"
    "60th\000"
    "61st\000"
    "62nd\000"
    "63rd\000"
    "64th\000"
    "65th\000"
    "66th\000"
    "67th\000"
    "68th\000"
    "69th\000"
    "70th\000"
    "71st\000"
    "72nd\000"
    "73rd\000"
    "74th\000"
    "75th\000"
    "76th\000"
    "77th\000"
    "78th\000"
    "79th\000"
    "80th\000"
    "81st\000"
    "82nd\000"
    "83rd\000"
    "84th\000"
    "85th\000"
    "86th\000"
    "87th\000"
    "88th\000"
    "89th\000"
    "90th\000"
    "91st\000"
    "92nd\000"
    "93th\000"
    "94th\000"
    "95th\000"
    "96th\000"
    "97th\000"
    "98th\000"
    "99th\000"
    }
};
