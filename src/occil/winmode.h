/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 * 
 */

#define CONSOLE 0 /* windows console, sets _WIN32 */
#define GUI 1     /* windows GUI, sets _WIN32 */
#define DLL 2     /* windows DLL, sets _WIN32 */
#define DOS 3     /* tran's pmode, no compatible file format, sets __DOS__ */
#define DOS32A 4  /* watcom/le style, use DOS32A as the extender, sets __DOS__ */
#define RAW 5     /* just a binary dump of the fixed up data, sets __RAW_IMAGE__ */
#define HXDOS 6   /* windows console, with HXDOS stub, sets __DOS__ && _WIN32 */
#define WHXDOS 7  /* dos/dpmi, using HXDOS , sets __DOS__ */
