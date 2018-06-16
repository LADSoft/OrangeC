HelpScribble project file.
16
YNQ`bsg-113363
0
2
OCIDE Help


LADSoft
TRUE

c:\orangec\src\help\pictures,C:\orangec\src\help\pictures\menu,C:\orangec\src\help\pictures\dialog,C:\orangec\src\help\pictures\contextmenu,C:\orangec\src\help\pictures\wnd,C:\orangec\src\help\pictures\generalproperties,c:\orangec\src\help\pictures\toolbar,c:\orangec\src\help\pictures\toolproperties,C:\orangec\src\help\pictures\dialog
1
BrowseButtons()
0
TRUE

FALSE
TRUE
16777215
0
32768
8388736
255
TRUE
TRUE
TRUE
FALSE
150
50
600
500
TRUE
FALSE
1
FALSE
FALSE
Contents
%s Contents
Index
%s Index
Previous
Next
FALSE

124
10
Scribble10
index
index;title;



Writing



FALSE
39
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\lang1033\b\fs32 Welcome to Orange C IDE help
\par 
\par \cf0\b0\fs20 Orange C IDE is an integrated development environment tailored to use with the Orange C tool set that supports C and C++ development and debugging.
\par 
\par Orange C IDE supports most functionality of the tool set; however it is not a standalone program.  To accomplish its tasks it usually spawns an appropriate tool from the tool set, such as the compiler or linker.  Output from spawned tools is integrated into the information window.
\par 
\par Orange C IDE may also be used as a standalone editor.
\par 
\par The following topics contain general information to help you get started:
\par 
\par \pard\li660\ri1440\cf2\strike Getting Started\cf3\strike0\{linkID=20\}
\par 
\par \pard\fi-680\li680\ri1440\cf0 Orange C IDE has the following general functions:
\par 
\par \pard{\pntext\f1\'B7\tab}{\*\pn\pnlvlblt\pnf1\pnindent0{\pntxtb\'B7}}\fi-680\li680\ri1440\cf2\strike Configurable Integrated editor\cf3\strike0\{linkID=50\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Project window, with integrated make facility\cf3\strike0\{linkID=190\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Resource editor\cf3\strike0\{linkID=1000\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Information windows\cf3\strike0\{linkID=280\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Error List\cf3\strike0\{linkID=295\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Variable Usages Window\cf3\strike0\{linkID=435\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Integrated debugger\cf3\strike0\{linkID=300\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Debugging windows\cf3\strike0\{linkID=320\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Find and Replace\cf3\strike0\{linkID=100\}
\par \cf2\strike{\pntext\f1\'B7\tab}Bookmarks\cf3\strike0\{linkID=400\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Integrated platform help\cf3\strike0\{linkID=420\}\cf0 
\par \cf2\strike{\pntext\f1\'B7\tab}Browse information\cf3\strike0\{linkID=430\}
\par \cf2\strike{\pntext\f1\'B7\tab}Printing\cf3\strike0\{linkID=440\}
\par \cf2\strike{\pntext\f1\'B7\tab}Menus\cf3\strike0\{linkID=40\}
\par \cf2\strike{\pntext\f1\'B7\tab}Toolbars\cf3\strike0\{linkID=450\}
\par \cf2\strike{\pntext\f1\'B7\tab}Hot keys\cf3\strike0\{linkID=850\}
\par \cf2\strike{\pntext\f1\'B7\tab}General Properties Dialog\cf3\strike0\{linkID=550\}
\par \cf2\strike{\pntext\f1\'B7\tab}Project Properties Dialog\cf3\strike0\{linkID=580\}
\par \pard\fi-680\li680\ri1440  
\par \cf1\b\fs32 For licensing information, look here:
\par 
\par \pard\li660\ri1440\cf2\b0\strike\fs20 GNU Version 3 License\cf3\strike0\{linkID=30\}\cf2\strike 
\par }
20
IDH_GETTING_STARTED
Getting Started
begin;getting started;start;



Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Getting Started\cf0\b0\f1\fs20 
\par 
\par \f0 Orange C IDE is a \cf2\strike workarea\cf3\strike0\{linkID=190\}\cf0  - based system.  That means to do anything significant other than simply \cf2\strike editing\cf3\strike0\{linkID=50\}\cf0  files (such as \cf2\strike compiling\cf3\strike0\{linkID=250\}\cf0  or \cf2\strike debugging\cf3\strike0\{linkID=300\}\cf0 ) a workarea must first be set up.  The workarea has information about \b Projects\b0 , which are executable or DLL files that are to be created.  A list of sources files is associated with each project; when attempting to create the project the IDE will apply the appropriate tool to each source file to compile it, then link the results together to create the project.  Orange C IDE maintains a list of dependency files and automatically rebuild all related source files when one of the dependency files changes.
\par 
\par The list of source files may be any combination of C/C++ language files, Assembly language files, RC files, and libraries.  Orange C IDE will automatically apply the compiler, the assembler, the resource compiler, or the linker depending on the file name extension of the source file.  If a \b Make\b0  option is selected, only files which have changed, or whose dependencies have changed, will be recompiled.  If the \b Build All\b0  option is selected Orange C IDE will rebuild all files irregardless of what has changed.
\par 
\par Creating a Workarea is a four-step process.  First, select\b  File -> Work Area -> New Workarea\b0  from the menu bar.  A \cf2\strike New Work Area Dialog\cf3\strike0\{linkID=362\}\cf0   prompts for the name of the workarea.  By default, this dialog will want to save your workarea in a new subdirectory under the \b My Documents\b0  folder, but this location can be changed.
\par 
\par After creating the Workarea, a project must be created.  To create a project select the \b File -> Work Area -> Add -> New Project\b0  menu item from the menu bar.  A \cf2\strike New Project Dialog\cf3\strike0\{linkID=360\}\cf0  prompts for the name and type of the project.  Type in the name of a project and select its type.  The project name should not have a file extension, as one will be added based on the type you select.  Normally, one would select a \b Console\b0  project for standard C programming, or text-based programs that are meant to be be accessed from a command prompt.  \b Windows GUI\b0  may be selected to make a program that is intended to have a windowing user interface.  The other project types are used for sharing code between projects.
\par 
\par Once the project is created, right-clicking on the project's name or on the source header will cause the \cf2\strike Project Context Menu\cf3\strike0\{linkID=230\}\cf0   to appear.   Press \b Add -> New File\b0  to create a new file to type in.  Later, when you attempt to save the file an Open File Dialog prompts you for the name to save it as.
\par 
\par If this is your first program and you have selected \b Console\b0  as the program type you might try typing something like:
\par 
\par \f2 #include <stdio.h>
\par 
\par int main(int argc, char *argv[])
\par \{
\par \tab printf("Hello World");
\par \}
\par \f0 
\par This short program is a classic first program that simply displays the text \cf1 Hello World\cf0  when you run it.
\par 
\par Selecting \b File -> Work Area -> Save\b0  from the menu bar will ensure the workarea and project is saved to disk.
\par 
\par At this point the project may be built by selecting one of the items on the \cf2\strike Build Menu\cf3\strike0\{linkID=250\}.\cf0   Once it is built it may be debugged by starting the \cf2\strike Integrated Debugger\cf3\strike0\{linkID=300\}\cf0  from either the \cf2\strike Debug Menu\cf3\strike0\{linkID=310\}\cf0  or the \cf2\strike Debug Toolbar\cf3\strike0\{linkID=760\}\cf0 .\f1 
\par }
30
Scribble30
Copyright
copyright;



Writing



FALSE
694
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\fs20                     GNU GENERAL PUBLIC LICENSE
\par                        Version 3, 29 June 2007
\par 
\par  Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
\par  Everyone is permitted to copy and distribute verbatim copies
\par  of this license document, but changing it is not allowed.
\par 
\par                             Preamble
\par 
\par   The GNU General Public License is a free, copyleft license for
\par software and other kinds of works.
\par 
\par   The licenses for most software and other practical works are designed
\par to take away your freedom to share and change the works.  By contrast,
\par the GNU General Public License is intended to guarantee your freedom to
\par share and change all versions of a program--to make sure it remains free
\par software for all its users.  We, the Free Software Foundation, use the
\par GNU General Public License for most of our software; it applies also to
\par any other work released this way by its authors.  You can apply it to
\par your programs, too.
\par 
\par   When we speak of free software, we are referring to freedom, not
\par price.  Our General Public Licenses are designed to make sure that you
\par have the freedom to distribute copies of free software (and charge for
\par them if you wish), that you receive source code or can get it if you
\par want it, that you can change the software or use pieces of it in new
\par free programs, and that you know you can do these things.
\par 
\par   To protect your rights, we need to prevent others from denying you
\par these rights or asking you to surrender the rights.  Therefore, you have
\par certain responsibilities if you distribute copies of the software, or if
\par you modify it: responsibilities to respect the freedom of others.
\par 
\par   For example, if you distribute copies of such a program, whether
\par gratis or for a fee, you must pass on to the recipients the same
\par freedoms that you received.  You must make sure that they, too, receive
\par or can get the source code.  And you must show them these terms so they
\par know their rights.
\par 
\par   Developers that use the GNU GPL protect your rights with two steps:
\par (1) assert copyright on the software, and (2) offer you this License
\par giving you legal permission to copy, distribute and/or modify it.
\par 
\par   For the developers' and authors' protection, the GPL clearly explains
\par that there is no warranty for this free software.  For both users' and
\par authors' sake, the GPL requires that modified versions be marked as
\par changed, so that their problems will not be attributed erroneously to
\par authors of previous versions.
\par 
\par   Some devices are designed to deny users access to install or run
\par modified versions of the software inside them, although the manufacturer
\par can do so.  This is fundamentally incompatible with the aim of
\par protecting users' freedom to change the software.  The systematic
\par pattern of such abuse occurs in the area of products for individuals to
\par use, which is precisely where it is most unacceptable.  Therefore, we
\par have designed this version of the GPL to prohibit the practice for those
\par products.  If such problems arise substantially in other domains, we
\par stand ready to extend this provision to those domains in future versions
\par of the GPL, as needed to protect the freedom of users.
\par 
\par   Finally, every program is threatened constantly by software patents.
\par States should not allow patents to restrict development and use of
\par software on general-purpose computers, but in those that do, we wish to
\par avoid the special danger that patents applied to a free program could
\par make it effectively proprietary.  To prevent this, the GPL assures that
\par patents cannot be used to render the program non-free.
\par 
\par   The precise terms and conditions for copying, distribution and
\par modification follow.
\par 
\par                        TERMS AND CONDITIONS
\par 
\par   0. Definitions.
\par 
\par   "This License" refers to version 3 of the GNU General Public License.
\par 
\par   "Copyright" also means copyright-like laws that apply to other kinds of
\par works, such as semiconductor masks.
\par 
\par   "The Program" refers to any copyrightable work licensed under this
\par License.  Each licensee is addressed as "you".  "Licensees" and
\par "recipients" may be individuals or organizations.
\par 
\par   To "modify" a work means to copy from or adapt all or part of the work
\par in a fashion requiring copyright permission, other than the making of an
\par exact copy.  The resulting work is called a "modified version" of the
\par earlier work or a work "based on" the earlier work.
\par 
\par   A "covered work" means either the unmodified Program or a work based
\par on the Program.
\par 
\par   To "propagate" a work means to do anything with it that, without
\par permission, would make you directly or secondarily liable for
\par infringement under applicable copyright law, except executing it on a
\par computer or modifying a private copy.  Propagation includes copying,
\par distribution (with or without modification), making available to the
\par public, and in some countries other activities as well.
\par 
\par   To "convey" a work means any kind of propagation that enables other
\par parties to make or receive copies.  Mere interaction with a user through
\par a computer network, with no transfer of a copy, is not conveying.
\par 
\par   "Target code" means any artifact generated by the Orange C compiler 
\par toolchain in the course of processing input files.
\par 
\par   "Independent modules" means any file processed by the Orange C compiler
\par toolchain while generating Target Code.
\par 
\par   The "Runtime Library" means the portions of the C/C++ libraries not covered
\par by external licensing, when processed into artifacts to be used while generating
\par Target Code.
\par 
\par   An interactive user interface displays "Appropriate Legal Notices"
\par to the extent that it includes a convenient and prominently visible
\par feature that (1) displays an appropriate copyright notice, and (2)
\par tells the user that there is no warranty for the work (except to the
\par extent that warranties are provided), that licensees may convey the
\par work under this License, and how to view a copy of this License.  If
\par the interface presents a list of user commands or options, such as a
\par menu, a prominent item in the list meets this criterion.
\par 
\par   1. Source Code.
\par 
\par   The "source code" for a work means the preferred form of the work
\par for making modifications to it.  "Object code" means any non-source
\par form of a work.
\par 
\par   A "Standard Interface" means an interface that either is an official
\par standard defined by a recognized standards body, or, in the case of
\par interfaces specified for a particular programming language, one that
\par is widely used among developers working in that language.
\par 
\par   The "System Libraries" of an executable work include anything, other
\par than the work as a whole, that (a) is included in the normal form of
\par packaging a Major Component, but which is not part of that Major
\par Component, and (b) serves only to enable use of the work with that
\par Major Component, or to implement a Standard Interface for which an
\par implementation is available to the public in source code form.  A
\par "Major Component", in this context, means a major essential component
\par (kernel, window system, and so on) of the specific operating system
\par (if any) on which the executable work runs, or a compiler used to
\par produce the work, or an object code interpreter used to run it.
\par 
\par   The "Corresponding Source" for a work in object code form means all
\par the source code needed to generate, install, and (for an executable
\par work) run the object code and to modify the work, including scripts to
\par control those activities.  However, it does not include the work's
\par System Libraries, or general-purpose tools or generally available free
\par programs which are used unmodified in performing those activities but
\par which are not part of the work.  For example, Corresponding Source
\par includes interface definition files associated with source files for
\par the work, and the source code for shared libraries and dynamically
\par linked subprograms that the work is specifically designed to require,
\par such as by intimate data communication or control flow between those
\par subprograms and other parts of the work.
\par 
\par   The Corresponding Source need not include anything that users
\par can regenerate automatically from other parts of the Corresponding
\par Source.
\par 
\par   The Corresponding Source for a work in source code form is that
\par same work.
\par 
\par   2. Basic Permissions.
\par 
\par   All rights granted under this License are granted for the term of
\par copyright on the Program, and are irrevocable provided the stated
\par conditions are met.  This License explicitly affirms your unlimited
\par permission to run the unmodified Program.  The output from running a
\par covered work is covered by this License only if the output, given its
\par content, constitutes a covered work.  This License acknowledges your
\par rights of fair use or other equivalent, as provided by copyright law.
\par 
\par   You may make, run and propagate covered works that you do not
\par convey, without conditions so long as your license otherwise remains
\par in force.  You may convey covered works to others for the sole purpose
\par of having them make modifications exclusively for you, or provide you
\par with facilities for running those works, provided that you comply with
\par the terms of this License in conveying all material for which you do
\par not control copyright.  Those thus making or running the covered works
\par for you must do so exclusively on your behalf, under your direction
\par and control, on terms that prohibit them from making any copies of
\par your copyrighted material outside their relationship with you.
\par 
\par   You have permission to propagate a work of Target Code formed by combining 
\par the Runtime Library with Independent Modules, even if such propagation would 
\par otherwise violate the terms of GPLv3. You may then convey such a combination 
\par under terms of your choice, consistent with the licensing of the Independent 
\par Modules.
\par 
\par   Conveying under any other circumstances is permitted solely under
\par the conditions stated below.  Sublicensing is not allowed; section 10
\par makes it unnecessary.
\par 
\par   3. Protecting Users' Legal Rights From Anti-Circumvention Law.
\par 
\par   No covered work shall be deemed part of an effective technological
\par measure under any applicable law fulfilling obligations under article
\par 11 of the WIPO copyright treaty adopted on 20 December 1996, or
\par similar laws prohibiting or restricting circumvention of such
\par measures.
\par 
\par   When you convey a covered work, you waive any legal power to forbid
\par circumvention of technological measures to the extent such circumvention
\par is effected by exercising rights under this License with respect to
\par the covered work, and you disclaim any intention to limit operation or
\par modification of the work as a means of enforcing, against the work's
\par users, your or third parties' legal rights to forbid circumvention of
\par technological measures.
\par 
\par   4. Conveying Verbatim Copies.
\par 
\par   You may convey verbatim copies of the Program's source code as you
\par receive it, in any medium, provided that you conspicuously and
\par appropriately publish on each copy an appropriate copyright notice;
\par keep intact all notices stating that this License and any
\par non-permissive terms added in accord with section 7 apply to the code;
\par keep intact all notices of the absence of any warranty; and give all
\par recipients a copy of this License along with the Program.
\par 
\par   You may charge any price or no price for each copy that you convey,
\par and you may offer support or warranty protection for a fee.
\par 
\par   5. Conveying Modified Source Versions.
\par 
\par   You may convey a work based on the Program, or the modifications to
\par produce it from the Program, in the form of source code under the
\par terms of section 4, provided that you also meet all of these conditions:
\par 
\par     a) The work must carry prominent notices stating that you modified
\par     it, and giving a relevant date.
\par 
\par     b) The work must carry prominent notices stating that it is
\par     released under this License and any conditions added under section
\par     7.  This requirement modifies the requirement in section 4 to
\par     "keep intact all notices".
\par 
\par     c) You must license the entire work, as a whole, under this
\par     License to anyone who comes into possession of a copy.  This
\par     License will therefore apply, along with any applicable section 7
\par     additional terms, to the whole of the work, and all its parts,
\par     regardless of how they are packaged.  This License gives no
\par     permission to license the work in any other way, but it does not
\par     invalidate such permission if you have separately received it.
\par 
\par     d) If the work has interactive user interfaces, each must display
\par     Appropriate Legal Notices; however, if the Program has interactive
\par     interfaces that do not display Appropriate Legal Notices, your
\par     work need not make them do so.
\par 
\par   A compilation of a covered work with other separate and independent
\par works, which are not by their nature extensions of the covered work,
\par and which are not combined with it such as to form a larger program,
\par in or on a volume of a storage or distribution medium, is called an
\par "aggregate" if the compilation and its resulting copyright are not
\par used to limit the access or legal rights of the compilation's users
\par beyond what the individual works permit.  Inclusion of a covered work
\par in an aggregate does not cause this License to apply to the other
\par parts of the aggregate.
\par 
\par   6. Conveying Non-Source Forms.
\par 
\par   You may convey a covered work in object code form under the terms
\par of sections 4 and 5, provided that you also convey the
\par machine-readable Corresponding Source under the terms of this License,
\par in one of these ways:
\par 
\par     a) Convey the object code in, or embodied in, a physical product
\par     (including a physical distribution medium), accompanied by the
\par     Corresponding Source fixed on a durable physical medium
\par     customarily used for software interchange.
\par 
\par     b) Convey the object code in, or embodied in, a physical product
\par     (including a physical distribution medium), accompanied by a
\par     written offer, valid for at least three years and valid for as
\par     long as you offer spare parts or customer support for that product
\par     model, to give anyone who possesses the object code either (1) a
\par     copy of the Corresponding Source for all the software in the
\par     product that is covered by this License, on a durable physical
\par     medium customarily used for software interchange, for a price no
\par     more than your reasonable cost of physically performing this
\par     conveying of source, or (2) access to copy the
\par     Corresponding Source from a network server at no charge.
\par 
\par     c) Convey individual copies of the object code with a copy of the
\par     written offer to provide the Corresponding Source.  This
\par     alternative is allowed only occasionally and noncommercially, and
\par     only if you received the object code with such an offer, in accord
\par     with subsection 6b.
\par 
\par     d) Convey the object code by offering access from a designated
\par     place (gratis or for a charge), and offer equivalent access to the
\par     Corresponding Source in the same way through the same place at no
\par     further charge.  You need not require recipients to copy the
\par     Corresponding Source along with the object code.  If the place to
\par     copy the object code is a network server, the Corresponding Source
\par     may be on a different server (operated by you or a third party)
\par     that supports equivalent copying facilities, provided you maintain
\par     clear directions next to the object code saying where to find the
\par     Corresponding Source.  Regardless of what server hosts the
\par     Corresponding Source, you remain obligated to ensure that it is
\par     available for as long as needed to satisfy these requirements.
\par 
\par     e) Convey the object code using peer-to-peer transmission, provided
\par     you inform other peers where the object code and Corresponding
\par     Source of the work are being offered to the general public at no
\par     charge under subsection 6d.
\par 
\par   A separable portion of the object code, whose source code is excluded
\par from the Corresponding Source as a System Library, need not be
\par included in conveying the object code work.
\par 
\par   A "User Product" is either (1) a "consumer product", which means any
\par tangible personal property which is normally used for personal, family,
\par or household purposes, or (2) anything designed or sold for incorporation
\par into a dwelling.  In determining whether a product is a consumer product,
\par doubtful cases shall be resolved in favor of coverage.  For a particular
\par product received by a particular user, "normally used" refers to a
\par typical or common use of that class of product, regardless of the status
\par of the particular user or of the way in which the particular user
\par actually uses, or expects or is expected to use, the product.  A product
\par is a consumer product regardless of whether the product has substantial
\par commercial, industrial or non-consumer uses, unless such uses represent
\par the only significant mode of use of the product.
\par 
\par   "Installation Information" for a User Product means any methods,
\par procedures, authorization keys, or other information required to install
\par and execute modified versions of a covered work in that User Product from
\par a modified version of its Corresponding Source.  The information must
\par suffice to ensure that the continued functioning of the modified object
\par code is in no case prevented or interfered with solely because
\par modification has been made.
\par 
\par   If you convey an object code work under this section in, or with, or
\par specifically for use in, a User Product, and the conveying occurs as
\par part of a transaction in which the right of possession and use of the
\par User Product is transferred to the recipient in perpetuity or for a
\par fixed term (regardless of how the transaction is characterized), the
\par Corresponding Source conveyed under this section must be accompanied
\par by the Installation Information.  But this requirement does not apply
\par if neither you nor any third party retains the ability to install
\par modified object code on the User Product (for example, the work has
\par been installed in ROM).
\par 
\par   The requirement to provide Installation Information does not include a
\par requirement to continue to provide support service, warranty, or updates
\par for a work that has been modified or installed by the recipient, or for
\par the User Product in which it has been modified or installed.  Access to a
\par network may be denied when the modification itself materially and
\par adversely affects the operation of the network or violates the rules and
\par protocols for communication across the network.
\par 
\par   Corresponding Source conveyed, and Installation Information provided,
\par in accord with this section must be in a format that is publicly
\par documented (and with an implementation available to the public in
\par source code form), and must require no special password or key for
\par unpacking, reading or copying.
\par 
\par   7. Additional Terms.
\par 
\par   "Additional permissions" are terms that supplement the terms of this
\par License by making exceptions from one or more of its conditions.
\par Additional permissions that are applicable to the entire Program shall
\par be treated as though they were included in this License, to the extent
\par that they are valid under applicable law.  If additional permissions
\par apply only to part of the Program, that part may be used separately
\par under those permissions, but the entire Program remains governed by
\par this License without regard to the additional permissions.
\par 
\par   When you convey a copy of a covered work, you may at your option
\par remove any additional permissions from that copy, or from any part of
\par it.  (Additional permissions may be written to require their own
\par removal in certain cases when you modify the work.)  You may place
\par additional permissions on material, added by you to a covered work,
\par for which you have or can give appropriate copyright permission.
\par 
\par   Notwithstanding any other provision of this License, for material you
\par add to a covered work, you may (if authorized by the copyright holders of
\par that material) supplement the terms of this License with terms:
\par 
\par     a) Disclaiming warranty or limiting liability differently from the
\par     terms of sections 15 and 16 of this License; or
\par 
\par     b) Requiring preservation of specified reasonable legal notices or
\par     author attributions in that material or in the Appropriate Legal
\par     Notices displayed by works containing it; or
\par 
\par     c) Prohibiting misrepresentation of the origin of that material, or
\par     requiring that modified versions of such material be marked in
\par     reasonable ways as different from the original version; or
\par 
\par     d) Limiting the use for publicity purposes of names of licensors or
\par     authors of the material; or
\par 
\par     e) Declining to grant rights under trademark law for use of some
\par     trade names, trademarks, or service marks; or
\par 
\par     f) Requiring indemnification of licensors and authors of that
\par     material by anyone who conveys the material (or modified versions of
\par     it) with contractual assumptions of liability to the recipient, for
\par     any liability that these contractual assumptions directly impose on
\par     those licensors and authors.
\par 
\par   All other non-permissive additional terms are considered "further
\par restrictions" within the meaning of section 10.  If the Program as you
\par received it, or any part of it, contains a notice stating that it is
\par governed by this License along with a term that is a further
\par restriction, you may remove that term.  If a license document contains
\par a further restriction but permits relicensing or conveying under this
\par License, you may add to a covered work material governed by the terms
\par of that license document, provided that the further restriction does
\par not survive such relicensing or conveying.
\par 
\par   If you add terms to a covered work in accord with this section, you
\par must place, in the relevant source files, a statement of the
\par additional terms that apply to those files, or a notice indicating
\par where to find the applicable terms.
\par 
\par   Additional terms, permissive or non-permissive, may be stated in the
\par form of a separately written license, or stated as exceptions;
\par the above requirements apply either way.
\par 
\par   8. Termination.
\par 
\par   You may not propagate or modify a covered work except as expressly
\par provided under this License.  Any attempt otherwise to propagate or
\par modify it is void, and will automatically terminate your rights under
\par this License (including any patent licenses granted under the third
\par paragraph of section 11).
\par 
\par   However, if you cease all violation of this License, then your
\par license from a particular copyright holder is reinstated (a)
\par provisionally, unless and until the copyright holder explicitly and
\par finally terminates your license, and (b) permanently, if the copyright
\par holder fails to notify you of the violation by some reasonable means
\par prior to 60 days after the cessation.
\par 
\par   Moreover, your license from a particular copyright holder is
\par reinstated permanently if the copyright holder notifies you of the
\par violation by some reasonable means, this is the first time you have
\par received notice of violation of this License (for any work) from that
\par copyright holder, and you cure the violation prior to 30 days after
\par your receipt of the notice.
\par 
\par   Termination of your rights under this section does not terminate the
\par licenses of parties who have received copies or rights from you under
\par this License.  If your rights have been terminated and not permanently
\par reinstated, you do not qualify to receive new licenses for the same
\par material under section 10.
\par 
\par   9. Acceptance Not Required for Having Copies.
\par 
\par   You are not required to accept this License in order to receive or
\par run a copy of the Program.  Ancillary propagation of a covered work
\par occurring solely as a consequence of using peer-to-peer transmission
\par to receive a copy likewise does not require acceptance.  However,
\par nothing other than this License grants you permission to propagate or
\par modify any covered work.  These actions infringe copyright if you do
\par not accept this License.  Therefore, by modifying or propagating a
\par covered work, you indicate your acceptance of this License to do so.
\par 
\par   10. Automatic Licensing of Downstream Recipients.
\par 
\par   Each time you convey a covered work, the recipient automatically
\par receives a license from the original licensors, to run, modify and
\par propagate that work, subject to this License.  You are not responsible
\par for enforcing compliance by third parties with this License.
\par 
\par   An "entity transaction" is a transaction transferring control of an
\par organization, or substantially all assets of one, or subdividing an
\par organization, or merging organizations.  If propagation of a covered
\par work results from an entity transaction, each party to that
\par transaction who receives a copy of the work also receives whatever
\par licenses to the work the party's predecessor in interest had or could
\par give under the previous paragraph, plus a right to possession of the
\par Corresponding Source of the work from the predecessor in interest, if
\par the predecessor has it or can get it with reasonable efforts.
\par 
\par   You may not impose any further restrictions on the exercise of the
\par rights granted or affirmed under this License.  For example, you may
\par not impose a license fee, royalty, or other charge for exercise of
\par rights granted under this License, and you may not initiate litigation
\par (including a cross-claim or counterclaim in a lawsuit) alleging that
\par any patent claim is infringed by making, using, selling, offering for
\par sale, or importing the Program or any portion of it.
\par 
\par   11. Patents.
\par 
\par   A "contributor" is a copyright holder who authorizes use under this
\par License of the Program or a work on which the Program is based.  The
\par work thus licensed is called the contributor's "contributor version".
\par 
\par   A contributor's "essential patent claims" are all patent claims
\par owned or controlled by the contributor, whether already acquired or
\par hereafter acquired, that would be infringed by some manner, permitted
\par by this License, of making, using, or selling its contributor version,
\par but do not include claims that would be infringed only as a
\par consequence of further modification of the contributor version.  For
\par purposes of this definition, "control" includes the right to grant
\par patent sublicenses in a manner consistent with the requirements of
\par this License.
\par 
\par   Each contributor grants you a non-exclusive, worldwide, royalty-free
\par patent license under the contributor's essential patent claims, to
\par make, use, sell, offer for sale, import and otherwise run, modify and
\par propagate the contents of its contributor version.
\par 
\par   In the following three paragraphs, a "patent license" is any express
\par agreement or commitment, however denominated, not to enforce a patent
\par (such as an express permission to practice a patent or covenant not to
\par sue for patent infringement).  To "grant" such a patent license to a
\par party means to make such an agreement or commitment not to enforce a
\par patent against the party.
\par 
\par   If you convey a covered work, knowingly relying on a patent license,
\par and the Corresponding Source of the work is not available for anyone
\par to copy, free of charge and under the terms of this License, through a
\par publicly available network server or other readily accessible means,
\par then you must either (1) cause the Corresponding Source to be so
\par available, or (2) arrange to deprive yourself of the benefit of the
\par patent license for this particular work, or (3) arrange, in a manner
\par consistent with the requirements of this License, to extend the patent
\par license to downstream recipients.  "Knowingly relying" means you have
\par actual knowledge that, but for the patent license, your conveying the
\par covered work in a country, or your recipient's use of the covered work
\par in a country, would infringe one or more identifiable patents in that
\par country that you have reason to believe are valid.
\par 
\par   If, pursuant to or in connection with a single transaction or
\par arrangement, you convey, or propagate by procuring conveyance of, a
\par covered work, and grant a patent license to some of the parties
\par receiving the covered work authorizing them to use, propagate, modify
\par or convey a specific copy of the covered work, then the patent license
\par you grant is automatically extended to all recipients of the covered
\par work and works based on it.
\par 
\par   A patent license is "discriminatory" if it does not include within
\par the scope of its coverage, prohibits the exercise of, or is
\par conditioned on the non-exercise of one or more of the rights that are
\par specifically granted under this License.  You may not convey a covered
\par work if you are a party to an arrangement with a third party that is
\par in the business of distributing software, under which you make payment
\par to the third party based on the extent of your activity of conveying
\par the work, and under which the third party grants, to any of the
\par parties who would receive the covered work from you, a discriminatory
\par patent license (a) in connection with copies of the covered work
\par conveyed by you (or copies made from those copies), or (b) primarily
\par for and in connection with specific products or compilations that
\par contain the covered work, unless you entered into that arrangement,
\par or that patent license was granted, prior to 28 March 2007.
\par 
\par   Nothing in this License shall be construed as excluding or limiting
\par any implied license or other defenses to infringement that may
\par otherwise be available to you under applicable patent law.
\par 
\par   12. No Surrender of Others' Freedom.
\par 
\par   If conditions are imposed on you (whether by court order, agreement or
\par otherwise) that contradict the conditions of this License, they do not
\par excuse you from the conditions of this License.  If you cannot convey a
\par covered work so as to satisfy simultaneously your obligations under this
\par License and any other pertinent obligations, then as a consequence you may
\par not convey it at all.  For example, if you agree to terms that obligate you
\par to collect a royalty for further conveying from those to whom you convey
\par the Program, the only way you could satisfy both those terms and this
\par License would be to refrain entirely from conveying the Program.
\par 
\par   13. Use with the GNU Affero General Public License.
\par 
\par   Notwithstanding any other provision of this License, you have
\par permission to link or combine any covered work with a work licensed
\par under version 3 of the GNU Affero General Public License into a single
\par combined work, and to convey the resulting work.  The terms of this
\par License will continue to apply to the part which is the covered work,
\par but the special requirements of the GNU Affero General Public License,
\par section 13, concerning interaction through a network will apply to the
\par combination as such.
\par 
\par   14. Revised Versions of this License.
\par 
\par   The Free Software Foundation may publish revised and/or new versions of
\par the GNU General Public License from time to time.  Such new versions will
\par be similar in spirit to the present version, but may differ in detail to
\par address new problems or concerns.
\par 
\par   Each version is given a distinguishing version number.  If the
\par Program specifies that a certain numbered version of the GNU General
\par Public License "or any later version" applies to it, you have the
\par option of following the terms and conditions either of that numbered
\par version or of any later version published by the Free Software
\par Foundation.  If the Program does not specify a version number of the
\par GNU General Public License, you may choose any version ever published
\par by the Free Software Foundation.
\par 
\par   If the Program specifies that a proxy can decide which future
\par versions of the GNU General Public License can be used, that proxy's
\par public statement of acceptance of a version permanently authorizes you
\par to choose that version for the Program.
\par 
\par   Later license versions may give you additional or different
\par permissions.  However, no additional obligations are imposed on any
\par author or copyright holder as a result of your choosing to follow a
\par later version.
\par 
\par   15. Disclaimer of Warranty.
\par 
\par   THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
\par APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
\par HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
\par OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
\par THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
\par PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
\par IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
\par ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
\par 
\par   16. Limitation of Liability.
\par 
\par   IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
\par WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
\par THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
\par GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
\par USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF
\par DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
\par PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),
\par EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
\par SUCH DAMAGES.
\par 
\par   17. Interpretation of Sections 15 and 16.
\par 
\par   If the disclaimer of warranty and limitation of liability provided
\par above cannot be given local legal effect according to their terms,
\par reviewing courts shall apply local law that most closely approximates
\par an absolute waiver of all civil liability in connection with the
\par Program, unless a warranty or assumption of liability accompanies a
\par copy of the Program in return for a fee.
\par 
\par                      END OF TERMS AND CONDITIONS
\par 
\par             How to Apply These Terms to Your New Programs
\par 
\par   If you develop a new program, and you want it to be of the greatest
\par possible use to the public, the best way to achieve this is to make it
\par free software which everyone can redistribute and change under these terms.
\par 
\par   To do so, attach the following notices to the program.  It is safest
\par to attach them to the start of each source file to most effectively
\par state the exclusion of warranty; and each file should have at least
\par the "copyright" line and a pointer to where the full notice is found.
\par 
\par     <one line to give the program's name and a brief idea of what it does.>
\par     Copyright (C) <year>  <name of author>
\par 
\par     This program is free software: you can redistribute it and/or modify
\par     it under the terms of the GNU General Public License as published by
\par     the Free Software Foundation, either version 3 of the License, or
\par     (at your option) any later version.
\par 
\par     This program is distributed in the hope that it will be useful,
\par     but WITHOUT ANY WARRANTY; without even the implied warranty of
\par     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
\par     GNU General Public License for more details.
\par 
\par     You should have received a copy of the GNU General Public License
\par     along with this program.  If not, see <https://www.gnu.org/licenses/>.
\par 
\par Also add information on how to contact you by electronic and paper mail.
\par 
\par   If the program does terminal interaction, make it output a short
\par notice like this when it starts in an interactive mode:
\par 
\par     <program>  Copyright (C) <year>  <name of author>
\par     This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
\par     This is free software, and you are welcome to redistribute it
\par     under certain conditions; type `show c' for details.
\par 
\par The hypothetical commands `show w' and `show c' should show the appropriate
\par parts of the General Public License.  Of course, your program's commands
\par might be different; for a GUI interface, you would use an "about box".
\par 
\par   You should also get your employer (if you work as a programmer) or school,
\par if any, to sign a "copyright disclaimer" for the program, if necessary.
\par For more information on this, and how to apply and follow the GNU GPL, see
\par <https://www.gnu.org/licenses/>.
\par 
\par   The GNU General Public License does not permit incorporating your program
\par into proprietary programs.  If your program is a subroutine library, you
\par may consider it more useful to permit linking proprietary applications with
\par the library.  If this is what you want to do, use the GNU Lesser General
\par Public License instead of this License.  But first, please read
\par <https://www.gnu.org/licenses/why-not-lgpl.html>.
\par \f1 
\par }
40
Scribble40
Menus
menu;



Writing



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Menus\cf2\b0\f1\fs20 
\par 
\par \f0 The IDE has two types of menus.  The first type is the menu bar at the top of the program's window interface.  The second type is context-sensitive menus that are available when right-clicking in the area of various special windows.
\par 
\par The menu bar at the top of the program appears as follows:
\par 
\par \pard\qc\cf3\{bmc menubar.png\}\cf2 
\par 
\par \pard It has the following menus:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-840\li840\tx4800\cf4\strike File menu\cf3\strike0\{linkID=60\}\tab\cf2 Shows file, workarea, and general menu items\cf3 
\par \cf4\strike{\pntext\f2\'B7\tab}Edit Menu\cf3\strike0\{linkID=130\}\tab\cf0 Shows items related to editing\cf2 
\par \cf4\strike\f1{\pntext\f2\'B7\tab}View Menu\cf3\strike0\{linkID=180\}\f0\tab\cf0 Shows the states of special windows, browse information, and bookmarks\cf3\f1 
\par \cf4\strike\f0{\pntext\f2\'B7\tab}Search Menu\cf3\strike0\{linkID=90\}\tab\cf0 Shows options related to searching for text\cf4 
\par \strike{\pntext\f2\'B7\tab}Project Menu\cf3\strike0\{linkID=110\}\cf0\tab Shows options related to the active project\cf4 
\par \strike\f1{\pntext\f2\'B7\tab}Build Menu\cf3\strike0\{linkID=250\}\f0\tab\cf0 Shows items related to compiling and linking programs\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Debug Menu\cf3\strike0\{linkID=310\}\cf0\f0\tab Shows items related to debugging
\par \cf4\strike{\pntext\f2\'B7\tab}Tools Menu\cf3\strike0\{linkID=120\}\cf0\tab Shows user defined tools and items related to customizing the IDE
\par \cf4\strike{\pntext\f2\'B7\tab}Resource Menu\cf3\strike0\{linkID=1020\}\cf0\tab Shows items related to resource editor windows\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Window Menu\cf3\strike0\{linkID=140\}\f0\tab\cf0 Shows items related to window management\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Help Menu\cf3\strike0\{linkID=260\}\f0\tab\cf0 Shows items related to documentation
\par \pard\fi-840\li840\tx4800 
\par The following additional context-sensitive menus are available by right-clicking on various windows in the resource editor:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-840\li840\tx4800\cf4\strike Accelerators Context Menu\cf3\strike0\{linkID=1050\}\cf0\tab Shows menu items for the Accelerator edit window
\par \cf4\strike{\pntext\f2\'B7\tab}Dialog Context Menu\cf3\strike0\{linkID=1070\}\cf0\tab Shows menu items for the Dialog edit window
\par \cf4\strike{\pntext\f2\'B7\tab}Image Context Menu\cf3\strike0\{linkID=1090\}\cf0\tab Shows menu items for the Bitmap, Cursor, and Icon edit windows
\par \cf4\strike{\pntext\f2\'B7\tab}Menu Context Menu\cf3\strike0\{linkID=1160\}\cf0\tab Shows menu items for the Menu edit window
\par \cf4\strike{\pntext\f2\'B7\tab}RCData Context Menu\cf3\strike0\{linkID=1110\}\cf0\tab Shows menu items for the RCData edit window
\par \cf4\strike{\pntext\f2\'B7\tab}String Table Context Menu\cf3\strike0\{linkID=1130\}\cf0\tab Shows menu items for the Strings edit window
\par \cf4\strike{\pntext\f2\'B7\tab}Version Context Menu\cf3\strike0\{linkID=1145\}\cf0\tab Show smenu items for the Version editor edit window
\par \pard\fi-840\li840\tx4800 
\par The following additional context-sensitive menus are available by right-clicking on various windows:\cf2\f1 
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-840\li840\tx4800\cf4\strike\f0 Client Context Menu\cf3\strike0\{linkID=195\}\tab\cf2 Shows items related to managing editor windows\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Editor Context Menu\cf3\strike0\{linkID=150\}\cf0\f0\tab Shows items related to editing, bookmarks, and debugging\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Information Context Menu\cf3\strike0\{linkID=290\}\cf0\f0\tab Shows items related to compiling and linking programs\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Watch Context Menu\cf3\strike0\{linkID=660\}\cf0\f0\tab Shows items related to the watch window\tab 
\par \cf4\strike{\pntext\f2\'B7\tab}Breakpoints Context Menu\cf3\{linkID=417\}\cf0\strike0\tab Shows items related to the breakpoints window
\par \cf4\strike{\pntext\f2\'B7\tab}Memory Context Menu\cf3\strike0\{linkID=690\}\cf0\tab Shows items relates to size of memory units\cf3\f1 
\par \cf4\strike{\pntext\f2\'B7\tab}Work\f0 area\f1  Context Menu\cf3\strike0\{linkID=210\}\cf0\f0\tab Shows items related to the workarea\cf3\f1 
\par \cf4\strike\f0{\pntext\f2\'B7\tab}Project Context Menu\cf3\strike0\{linkID=230\}\cf0\tab Shows Items related to the selected project\cf3 
\par \cf4\strike{\pntext\f2\'B7\tab}Folder Context Menu\cf3\strike0\{linkID=220\}\cf0\tab Shows items related to the selected folder\cf3 
\par \cf4\strike{\pntext\f2\'B7\tab}File Context Menu\cf3\strike0\{linkID=240\}\cf0\tab Shows items related to the selected file
\par \pard\fi-840\li840\tx4800\cf3\f1 
\par \cf2 
\par }
50
Scribble50
Editor Window
editor window;windows;



Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Editor Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Editor Window iss accessible by opening a file, either by selecting FILE->FILE->OPEN from the \cf2\strike Main <enu\cf3\strike0\{linkID=40\}\cf0  or by clicking on the file in the \cf2\strike Workarea Window\cf3\strike0\{linkID=190\}\cf0 .\f1 
\par 
\par \pard\ri1440\tx660\tx2820\f0 The editor is a colorizing editor with basic functionality similar to a rich edit control, and various extended functionality added to support various IDE functionality.  It can colorize for the C language, NASM, and Resource files.  Colorization is done based on file extension.  \cf2\strike Configuration\cf3\strike0\{linkID=550\}\cf0  of basic functionality is supported.  
\par 
\par The editor window appears similar to what follows:
\par 
\par \pard\ri1440\qc\tx660\tx2820\cf3\{bmc debugsession.png\}\cf0 
\par \pard\ri1440\tx660\tx2820 
\par In this example, the \cf2\strike Integrated Debugger\cf3\strike0\{linkID=300\}\cf0  has been started.  The blue bar to the left of the text shows code lines which have generated code associated with them, and are suitable for breakpoints.  The red circle is a break point.  The yellow arrow shows the position at which the program flow was stopped.  This example also shows a bookmark, which is a grey square.
\par 
\par When one or more files are opened for editing, a series of tabs at the top of the client area will show a list of all open files, and allow easy navigation to a given file.  If there are two many open files, an arrow button to the far right of the tabs will allow opening a menu to select from available files.  A star next to a file name indicates the file has changed and needs to be saved.
\par 
\par Above all the editor windows but below the tab with the file names is the \cf2\strike Jump List\cf3\strike0\{linkID=900\}\cf0 .  The box at the left selects a type of variable such as a global functions or global variables, and the box at the right gives a list of function or variable names.  Selecting a function or variable name from the box at the right opens a window for the associated file (if it isn't already open) and positions the cursor on the line where the variable or function is declared.
\par 
\par Other ways to navigate to an open window is to view the window list on the \cf2\strike Window menu\cf3\strike0\{linkID=140\}\cf0 , and to use the \cf2\strike Client Context Menu\cf3\strike0\{linkID=195\}\cf0  to open the \cf2\strike Select Window\cf3\strike0\{linkID=330\}\cf0  dialog.
\par 
\par Various menu items are available to help manage the editor windows.  These include items on the \cf2\strike File menu\cf3\strike0\{linkID=60\}\cf0  of the main menu, on the \cf2\strike Edit menu\cf3\strike0\{linkID=130\}\cf0  of the main menu, the Windows menu, the Client Context Menu, and the \cf2\strike Context menu\cf3\strike0\{linkID=150\}\cf0  available in edit windows.
\par \pard\fi-2780\li2780\ri1440\tx680\tx2820 
\par \pard\ri1440\tx660\tx2820 Double clicking in the area to the left of the window will toggle a \cf2\strike Breakpoint\cf3\strike0\{linkID=410\}\cf0  wit the selected line.
\par 
\par Pressing SHIFT-F1 while an edit window is selected will bring up C runtime library help for the word under the cursor.
\par \pard\fi-2780\li2780\ri1440\tx680\tx2820 
\par \pard\ri1440\tx660\tx2820 In the background, the IDE will parse through files that are in open edit windows, to get lists of type information and structure elements.  This information is used for
\par code completion, and to show hints about variable types.
\par }
60
Scribble60
File Menu
file;file menu;menu;



Writing



FALSE
43
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 File Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The File Menu has menu items related to files and some generic commands.  These include the file and workarea submenus.  It appears similar to what follows:
\par 
\par \pard\qc 
\par \cf3\{bmc mfile.png\}\cf2 
\par \pard 
\par \b Open Work Area\b0  is used to browse to a work area to open
\par 
\par \b New Work Area\b0  creates a new Work Area
\par 
\par \b Reopen Work Area\b0  opens a menu which shows recent Work Areas that have been opened.   Clicking on one opens it.
\par 
\par \b Open Existing Project\b0  brings an existing project into the work area
\par 
\par \b New Project\b0  creates a new project in the work area
\par 
\par \b Open Existing File\b0  browses to a source file to open
\par 
\par \b New File\b0  creates a new source file
\par 
\par \b Reopen File\b0  opens a menu which shows recent source files which have been opened.   Clicking on one opens it.
\par 
\par \b Close File\b0  closes the current edit window
\par 
\par \b Save Work Area\b0  saves the Work Area and Projects if they have changed
\par 
\par \b Save All Files\b0  saves changes to all open source files
\par 
\par \b Save File\b0  saves changes to the source file which is currently being edited
\par 
\par \b Print\b0  is used to \cf4\strike print\cf3\strike0\{linkID=440\}\cf2  files. It opens the standard windows printer dialog and allows selection of various properties related to the print request.
\par 
\par \b Import\b0  is used to bring old CC386 workspaces and projects forward.
\par 
\par \b Command Window \b0 opens a command prompt.  The command prompt will have sufficient environment variables defined to build the project.
\par 
\par \b Exit\b0  exits the IDE.  If any files need to be saved or the debugger needs to be closed, a prompt will appear requesting confirmation.
\par 
\par 
\par }
90
Scribble90
Search Menu




Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Search Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The Search Menu shows options related to searching for text, and replacing one text with another.  It appears similar to what follows:
\par 
\par \pard\qc\cf2\{bmc msearch.png\}\cf0 
\par \pard 
\par \b Find\b0  brings up a \cf3\strike Find/Replace\cf2\strike0\{linkID=100\}\cf0  dialog configured for locating text in editor windows.
\par \b 
\par Replace\b0  brings up a Find/Replace dialog configured for replacing text in editor windows.
\par 
\par \b Find Next\b0  locates the next occurrance of the last text searched for.
\par 
\par \b Find In Files\b0  brings up a Find/Replace dialog configured for locating text in files that are not in editor windows.
\par \f1 
\par }
100
IDH_FIND_REPLACE_DIALOG
Find/Replace dialog




Writing



FALSE
97
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Find/Replace Dialog\cf0\b0\f1\fs20 
\par 
\par \pard\ri1440\tx660\f0 The Find/Replace dialog is accessible by clicking one of the items on the \cf2\strike Edit Toolbar\cf3\strike0\{linkID=740\}\cf0 , by selecting one of the items from the \cf2\strike Search Menu\cf3\strike0\{linkID=90\}\cf0  , or by pressing \b CTRL-F\b0  or \b CTRL-H\b0 .\f1 
\par \pard 
\par \f0 The Find/Replace dialog is a generic dialog with two tabs.  One tab is used for locating text, and the other tab is used for replacing text.  The text may be searched for in a variety of places, including in open windows, as part of a project or workarea, or on the disk.  Text that is found may either be found one item at a time with navigation to each item, or a batch of locations may be sent to one of two windows dedicated to showing text that has been located.
\par 
\par When used to locate text, the Find/Replace dialog appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc find.png\}\cf0 
\par \pard 
\par Note that some items will sometimes be closed to make the dialog smaller.
\par \f1 
\par \b\f0 Find What\b0  allows selection of the text to locate
\par 
\par \b Type\b0  allows selection of a filter to be used for file names, by default all files are searched but the scope may be narrowed to various types of files the IDE understands if necessary.
\par 
\par \b Where\b0  allows selection of what documents are in the scope of the search.  This may be one of the following
\par 
\par \pard\fi-3120\li4220\tx4260\b Current document\b0\tab any text in the window that currently has focus
\par \b Open documents\b0\tab any text in any open window
\par \b Current project\b0\tab any text in any file within the selected project
\par \b All projects\b0\tab any text in any file that exists in the workarea
\par \b Orange C include path\b0\tab any text in system include files
\par \b User specified path\b0\tab any text in a user-specified path.
\par \pard\fi1120\tx4260 
\par \pard\b Path\b0  allows selection of a path to look in for files, when Where is set to User Specified Path.  The button at the right will bring up a window that allows selection of the path, or the path may be directly typed in.  Otherwise this window doesn't appear.
\par 
\par \b How\b0  specifies how matches will be calculated.
\par 
\par \pard\fi-3080\li4180\tx4200\b Direct Match\b0\tab characters must directly match\b 
\par Regular Expressions\b0\tab\cf2\strike regular expressions\cf3\strike0\{linkID=810\}\cf0  may be used to determine the match\b 
\par Wildcard\b0\tab characters generally directly match, however \b ?\b0  may be used to match any character and \b *\b0  may be used to match any sequence of characters\b 
\par \pard\fi1160\tx4240 
\par \pard Options\b0  allow for various special case behaviors while searching.  They include
\par 
\par \pard\fi-3060\li4200\tx4180\b Match Whole Word\b0\tab when selected, any text that is matched must be the same word as \b Find What -\b0  no matches will be made on partial words
\par \b Match Case\b0\tab when selected, any text that is matched must be the same case as specified in \b  What\b0 
\par \b Recursive\b0\tab when selected in conjunction with a \b User Specified Path\b0 , will cause the search to extend to subdirectories
\par \b Search Up\b0\tab when selected, the search will be done toward the beginning of the documents instead of the end.
\par \pard\fi1140\tx4260 
\par \f1 
\par \pard\b\f0 Output\b0  selects what is going to happen when something is located.
\par 
\par \pard\fi-3060\li4200\tx4220\b Document Window\b0\tab navigate to the next location found and then stop locating text\b 
\par Find Window 1\b0\tab show the location in Find Window 1 and continue locating text\b 
\par Find Window 2\tab\b0 show the location in Find Window 2 and continue locating text\b 
\par 
\par Find Window 1 \b0 and\b  Find Window 2\b0  may be selected from tabs on the \cf2\strike Information Window\cf3\strike0\{linkID=280\}\cf0\b 
\par \pard\b0 
\par Note that for each type of location that may be specified in \b Where\b0 , options and output selections will be remembered independently.
\par \f1 
\par \f0 There are two find windows, find-related information may be sent to either window.\f1 
\par 
\par \f0 When used to replace text, the Find/Replace dialog appears as follows:
\par 
\par \pard\qc\cf3\f1\{bmc replace.png\}\cf0 
\par \pard 
\par \f0 Note that some items will sometimes be closed to make the dialog smaller.
\par \f1 
\par \b\f0 Find What\b0  allows selection of the text to locate
\par 
\par \b Replace With\b0  allows selection of the text to replace it with
\par 
\par \b Type\b0  allows selection of a filter to be used for file names, by default all files are searched but the scope may be narrowed to various types of files the IDE understands if necessary.
\par 
\par \b Where\b0  allows selection of what documents are in the scope of the search.  This may be one of the following
\par 
\par \pard\fi-3140\li4240\tx4260\b Current document\b0\tab any text in the window that currently has focus
\par \b Open documents\b0\tab any text in any open window
\par \b Current project\b0\tab any text in any file within the selected project
\par \b All projects\b0\tab any text in any file that exists in the workarea
\par \b Orange C include path\b0\tab any text in system include files
\par \b User specified path\b0\tab any text in a user-specified path.
\par \pard\fi1120\tx4260 
\par \pard\b Path\b0  allows selection of a path to look in for files, when Where is set to User Specified Path.  The button at the right will bring up a window that allows selection of the path, or the path may be directly typed in.  Otherwise this window doesn't appear.
\par 
\par \b How\b0  specifies how matches will be calculated.
\par \pard\fi-3100\li4240\tx4240 
\par \pard\fi-3120\li4200\tx4200\b Direct Match\b0\tab characters must directly match\b 
\par Regular Expressions\b0\tab\cf2\strike regular expressions\cf3\strike0\{linkID=810\}\cf0  may be used to determine the match\b 
\par Wildcard\b0\tab characters generally directly match, however \b ?\b0  may be used to match any character and \b *\b0  may be used to match any sequence of characters\b 
\par \pard\fi-3100\li4240\tx4240 
\par \pard Options\b0  allow for various special case behaviors while searching.  They include
\par 
\par \pard\fi-3100\li4240\tx4240\tx4260\b Match Whole Word\b0\tab when selected, any text that is matched must be the same word as \b Find What -\b0  no matches will be made on partial words
\par \b Match Case\b0\tab when selected, any text that is matched must be the same case as specified in \b Find What\b0 
\par \b Recursive\b0\tab when selected in conjunction with a \b User Specified Path\b0 , will cause the search to extend to subdirectories
\par \pard\fi1140\tx4260 
\par \f1 
\par \pard\f0 When performing a single replace, the IDE will navigate to the location that is found and show a single replacement.  When doing a Replace All, all replacements will be made by loading the files selected by 'where' into editor windows as needed, and making the changes without saving them.  The changes may then be reviewed before being accepted...
\par 
\par Note that for each type of location that may be specified in \b Where\b0 , options and output selections will be remembered independently.
\par \f1 
\par 
\par }
105
Scribble105
Find Windows




Writing



FALSE
13
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Find Windows\cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 The find windows serve as targets for find and replace.
\par 
\par \pard\ri1440\qc\tx660\tx2820\cf2\{bmc wfind.png\}\cf0 
\par \pard\lang1049\f1 
\par \lang1033\f0 In the example the search was for the word 'arg'.   It found a number of matches which are listed in the window.   Clicking on a match takes one to the line of source code for the match.
\par 
\par The various icons at the top of the window allow navigating in the window, and searching for text in the window.
\par \lang1049\f1 
\par }
110
Scribble110
Project Menu
dependencies;



Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Project Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The Project Menu has items related to projects.  It appears similar to what follows:
\par 
\par \pard\qc\cf2\{bmc mproject.png\}\cf0 
\par \pard\f1 
\par \b\f0 Create New Project\b0  brings up the \cf3\strike New Project Dialog\cf2\strike0\{linkID=360\}\cf0  to allow entry of parameters for creating a new project.
\par 
\par \b Add Existing Project\b0  brings up an \cf3\strike Open File Dialog\cf2\strike0\{linkID=350\} \cf0 to allow locating an existing project to load into the current workarea.
\par 
\par \cf4\b Project Dependencies\b0  brings up the \cf3\strike Project Dependencies dialog\cf2\strike0\{linkID=582\}\cf4  to allow selection of which projects depend on other projects.
\par 
\par \b Select Profile\b0  opens the \cf3\strike Select Profile dialog\cf2\strike0\{linkID=589\}\cf4  to allow selection of the current \cf3\strike profile\cf2\strike0\{linkID=585\}\cf4\b 
\par \cf0\b0 
\par \b Project Properties\b0  opens the \cf3\strike project properties dialog\cf2\strike0\{linkID=580\}\cf0  for the active project.\f1 
\par }
120
Scribble120
Tools Menu




Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Tools Menu
\par \cf0\b0\fs20 
\par The tools menu has options for external tools, and for configuring the IDE.  It appears similar to what follows:
\par 
\par \pard\qc\cf2\{bmc mtools.png\}\cf0 
\par \pard\f1 
\par \f0 In this case an external tool which has been labeled '\cf1 my tool'\cf0  appears on the\b  External Tools\b0  submenu.  When selected, this will perform a specific function which has been configured through the \cf3\strike Customize\cf2\strike0\{linkID=910\}\cf0  option on the \b External Tools\b0  submenu.  Other external tools may be configured or removed through this option.
\par 
\par \b Customize\b0  (on the tools menu) brings up the \cf3\strike Toolbar Customization dialog\cf2\strike0\{linkID=860\}\cf0  which allows configuration of which toolbars will be shown, as well as the configuration of what buttons will be shown and in what order.
\par 
\par \b Build rules\b0  brings up the \cf3\strike Build Rules dialog\cf2\strike0\{linkID=880\}\cf0 , which allows customization of build rules.
\par 
\par \b Properties\b0  brings up the \cf3\strike General Properties\cf2\strike0\{linkID=550\}\cf0  page, which allows configuration of the IDE.\f1 
\par }
130
Scribble130
Edit Menu
edit menu;menu;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Edit Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Edit Menu has items which allow browsing and manipulating the text in the edit windows.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc medit.png\}\cf2 
\par \pard 
\par \b Cut, Copy\b0  and \b Paste\b0  are used to move selected text to and from the clipboard.  
\par 
\par \b Undo\b0  undoes recently made edits.  The Undo buffer is fairly deep. Note that you can undo changes even after you save a file, until the file window is closed.
\par 
\par \b Redo\b0  redoes changes that have recently been undone.
\par 
\par \b Select All\b0  selects all the text in the current edit window.
\par 
\par \b Uppercase\b0  and \b Lowercase\b0  change the selected text to either upper or lower case.
\par 
\par \pard\ri1440\b Indent\b0  moves the selected right text by one tab space.  \cf0 This may uses either spaces or tabs depending on the \cf4\strike Editor Formatting Configuration\cf3\strike0\{linkID=510\}\cf0 
\par \pard\cf2 
\par \b Unindent\b0  is similar to \b indent\b0 , except it moves the text left.\f1 
\par \cf0\f0 
\par \b Comment\b0  adds comment markers to the beginning of each line in a selected block of text, to make the text inactive code.
\par 
\par \b Uncomment\b0  removes comment markers to the beginning of each line in a selected block of text, to make the text active code again.
\par }
140
Scribble140
Window Menu
menu;window menu;



Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Window Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Window Menu has various items used for controlling windows in the client area of the IDE.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc mwindow.png\}
\par \pard 
\par \cf2\b Close\b0  closes the currently selected window.
\par 
\par \b Close All\b0  closes all edit windows on the client area, but not special windows that have been undocked.
\par 
\par \b Save All Files\b0  saves changes to the files in all open windows.
\par 
\par \b Cascade\b0  orders all client area windows so that they are the same size and are overlapped in a staggered fashion, where the title bar of each successive window is just below and to the right of the previous window.
\par 
\par \b Tile Horizontally\b0  orders all client area windows so that they are stacked from the top of the client area to the bottom
\par 
\par \b Tile Vertically\b0  orders all client area windows so that they are stacked from the left of the client area to the right.
\par 
\par \b Arrange Icons\b0  organizes any client area windows that have been iconized.
\par 
\par The remaining elements on this menu show some of the files that are open.  The currently selected file will have a star next to it.  If there are too many open files a \b\i More Windows...\b0\i0   item will appear, which opens the \cf4\strike Select Window Dialog\cf3\strike0\{linkID=330\}.\cf0   This allows navigation to any window shown in the client area.
\par \cf2\f1 
\par \cf0\f0 Special Windows such as the Watch Window will also appear in the client area if undocked, and will show up in this list.
\par \cf2\f1 
\par \f0 Window navigation may also be performed by using the tabs at the top of the client area.
\par \f1 
\par }
150
Scribble150
Editor Context Menu
context;edit context menu;menu;



Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Editor Context Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Edit Context Menu iss accessible by right-clicking in one of the \cf3\strike Editor Windows\cf4\strike0\{linkID=50\}\cf2 . It has menu items which relate to editing, browsing, and debugging.  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc cmedit.png\}\cf2 
\par \pard 
\par \pard\ri1440\tx660\tx2820\cf0\b 
\par Browse To Definition \b0 uses \cf3\strike Browse Information\cf4\{linkID=430\}\cf0\strike0  to browse to the definition of a variable or function 
\par 
\par \b Browse To Declaration\b0  uses Browse Information to browse to the declaration of a variable or function\b 
\par \b0 
\par \b Toggle Bookmark\b0  toggles a \cf3\strike Bookmark\cf4\strike0\{linkID=400\}\cf0  at the selected line
\par 
\par \pard\cf2\b Goto line\b0  opens the \cf3\strike Goto Dialog\cf4\strike0\{linkID=340\}\cf2 .  This allows positioning on a given line when the line number is entered.
\par \pard\ri1440\tx660\tx2820\cf0 
\par \b Return To Origin\b0  opens the window the current breakpoint is associated with and navigates to the breakpoint line.
\par \b 
\par Toggle Breakpoint\b0  toggles the break point setting for the current line.  See \cf3\strike Breakpoints\cf4\strike0\{linkID=410\}\cf0  for further information about breakpoints.
\par 
\par \b Data Breakpoint\b0  opens the \cf3\strike Data Breakpoint\cf4\strike0\{linkID=890\}\cf0  dialog to allow adding a data breakpoint on the word under the cursor.
\par 
\par \b Run To Cursor\b0  starts the program being debugged if it is currently stopped, and keeps it running until either a  is hit or the flow of execution reaches the line with the cursor.
\par 
\par \b Add To Watch\b0  adds the identifier under the cursor to the \cf3\strike Watch Window\cf4\strike0\{linkID=650\} \cf0 and shows its value.  The Watch Window will be opened if it is currently closed.
\par 
\par \b Save File\b0  saves the file in the edit window
\par }
160
Scribble160
View Bookmarks Menu
bookmarks;menu;view;view bookmarks menu;



Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 View Bookmarks Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The View Bookmarks Menu has menu items related to \cf3\strike Bookmarks\cf4\strike0\{linkID=400\}\cf2 .  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc mbookmarks.png\}\cf2 
\par \pard 
\par \b Toggle Bookmark \b0 toggles the bookmark state of the line currently selected in the editor window.
\par 
\par \b Next Bookmark \b0 moves the cursor to the line of the next bookmark in the bookmarks list.  If necessary it opens a new edit window with the related file.
\par 
\par \b Previous Bookmark \b0 moves the cursor to the line of the previous bookmark in the bookmarks list.  If necessary it opens a new edit window with the related file.\f1 
\par 
\par \b\f0 Next File\b0  moves the cursor forward past all the bookmarks left in the current file, and positions to the first bookmark in the next file in the list.
\par 
\par \b Previous File\b0  moves the cursor backward past all the previous bookmarks in the current file, and positions to the first bookmark in the previous file in the list.
\par \f1 
\par \b\f0 Show Bookmarks\b0  opens the \cf3\strike Bookmark Dialog\cf4\strike0\{linkID=390\}\cf2 .  This allows the ability to browse to a specific bookmark.
\par 
\par \b Remove All Bookmarks\b0  removes all bookmarks from the bookmarks list.\f1 
\par }
170
Scribble170
View Browse Menu
menu;view;view browse menu;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 View Browse Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The View Browse Menu has menu items related to \cf3\strike Browse Information\cf4\strike0\{linkID=430\}\cf2 .  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc mbrowse.png\}\cf2 
\par \pard 
\par \b Browse To Definition\b0  browses to the definition of the word under the cursor using \cf3\strike Browse Information\cf4\strike0\{linkID=430\}\cf2 
\par 
\par \b Browse To Declaration\b0  browses to the declaration of the word under the cursor using Browse Information\b 
\par 
\par Browse To\b0  opens the \cf3\strike Browse To Dialog\cf4\strike0\{linkID=380\}\cf2 .  This allows selecting a variable name to locate the definition for.
\par 
\par \b Browse Back\b0  undoes the last \b Browse \b0 or \b Browse To \b0 command.\f1 
\par }
180
Scribble180
View Menu
menu;view;view menu;



Writing



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 View Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The View Menu has various items which allow for viewing windows or positioning to bookmarks and browsing.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc mview.png\}\cf2 
\par \pard 
\par \b Workarea\b0  opens or closes the \cf4\strike Workarea Window\cf3\strike0\{linkID=190\}. \cf2 When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Information Window\b0  opens or closes the \cf4\strike Information Window\cf3\strike0\{linkID=280\}\cf2 . When the window is displayed a check mark will appear next to the menu item.
\par \f1 
\par \b\f0 Create New View \b0 makes a copy of an existing editor window.  The new window will be a new view that looks at the same data as the existing view.  Changes in one view will be reflected in the other.  This mechanism allows one to view two separate locations in the same file.
\par \f1 
\par \b\f0 Bookmarks\b0  opens the \cf4\strike View Bookmarks Menu\cf3\strike0\{linkID=160\}\cf2 .
\par 
\par \b Browse\b0  opens the \cf4\strike View Browse Menu\cf3\strike0\{linkID=170\}.
\par 
\par \cf0\b Goto Line\cf2\b0  opens the \cf4\strike Goto Line\cf3\strike0\{linkID=340\}\cf2  dialog.  This allows selection of a line number to navigate to..
\par 
\par \b View Other\b0  opens a submenu that looks similar to what follows:
\par 
\par 
\par \pard\qc\cf3\{bmc mviewother.png\}\cf2 
\par \pard 
\par On this menu,
\par 
\par \b Error List\b0  opens or closes the \cf4\strike Error List Window\cf3\strike0\{linkID=295\}\cf2 .   When the window is displayed a check mark will appear next to the menu item.
\par \f1 
\par \b\f0 Property Window\b0  opens or closes the \cf4\strike Resourced Property Window\cf3\strike0\{linkID=1026\}. \cf0  This window shows information about resources being edited.
\par \cf2 
\par \b Variable Usages\b0  opens or closes the \cf4\strike Variable Usages Window\cf3\strike0\{linkID=435\}\cf2 .   This window uses browse information to show information about the usages of a variable or function, such as where it is declared and defined, and where it is used.
\par 
\par \b Resources\b0  opens or closes the \cf4\strike Resource Window\cf3\strike0\{linkID=1005\}\cf2 .
\par 
\par \b Control Toolbar\b0  opens the\cf4\strike  Dialog Control Toolbox \cf3\strike0\{linkID=1065\},\cf0   which chooses controls to place on a dialog.
\par \cf2 
\par }
190
Scribble190
Workarea Window
workspace;workspace window;



Writing



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\ri1420\tx700\tx2800\cf1\b\fs32 Workarea Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Workarea Window iss accessible by clicking Work Area from the \cf2\strike View Menu\cf3\strike0\{linkID=180\}\cf0 .  It holds a list of projects to be created.  \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\ri1420\qc\tx700\tx2800\cf3\{bmc wworkarea.png\}\cf0 
\par \pard\ri1420\tx700\tx2800 
\par 
\par Each project is an executable, DLL, or static library file.  Executables are programs that can be run. DLLs are like executables except they hold shared code that executables can access.  Static libraries hold code that is not itself executable but can be combined with other code to make an executable.
\par 
\par When there are multiple projects, one project will always be selected as the active project.  This is the project that will be used by default when an action such as an attempt to debug occurs.
\par 
\par Underneath each project is a list of all the source files which are required to create the project.  When a build is requested the IDE parses through the list of files to see which ones have changed, and spawns an appropriate compiler for each one.  For file types it knows about such as C language files, the IDE will automatically parse the files to make a list of dependencies.
\par 
\par New projects may be created in the workarea using the \cf2\strike Workarea Menu\cf3\strike0\{linkID=70\}\cf0  or the \cf2\strike Workarea Context Menu\cf3\strike0\{linkID=210\}\cf0 .  There are also commands to import pre-existing projects.
\par 
\par Once a project is created, a few tree items will appear underneath it.  These items hold the source files for the project.  By using the Workarea Context Menu, new files can be inserted underneath each header.  Additionally, folders may be added to organize the files.  Folders and files may be dragged around to change the heirarchy.
\par 
\par Note that the folders are not related to how data is stored in the file system.
\par 
\par When adding files to the project, the IDE will attempt to classify them and put them in one of the stock folders, which are \b Source Files\b0 , \b Include Files,\b0   and \b Resource Files\b0 .  If the associated folder has been removed it will be added again to have a place to put the file.  Once files are added they can be moved to another folder within the project.
\par 
\par Note that the \b Include Files\b0  folder is a convenience that allows one to access include files from the IDE.  The IDE will automatically calculate the dependencies of source files for purposes of the build process.  This is unrelated to the contents of the \b Include Files\b0  folder, which must be manually populated if it is desired to have any files there.
\par 
\par After a file is added to the project, double clicking on a file or selecting it and pressing enter will open that file in an editor window.
\par 
\par If a project is a library file, other projects will automatically be linked against it.
\par 
\par The workarea file stores various general settings related to the user interface, and a reference to a separate configuration file for each project.  Each project may appear in multiple workareas.  The paths stored in the configuration files are relative.  That means that projects may be moved around on the disk as long the workareas and projects remain in the same relative location to each other.
\par 
\par The arrow keys can be used to navigate through this menu.  The ENTER key opens the file the cursor is over.  CTL-INSERT and CTL-DELETE can be used to open and close sub trees.  
\par 
\par INSERT and DELETE can be used to add and remove projects and files.  INSERT opens the \cf2\strike Open File Dialog\cf3\strike0\{linkID=350\}\cf0  to allow selection of file names or new files to add.  DELETE will only display a warning if an attempt is made to remove a project; removing a file succeeds silently
\par 
\par Any project or file that is removed from the workarea still exists on the disk, and can be added again at a later time.
\par 
\par }
195
Scribble195
Client Context Menu
client context menu;context;menu;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Client Context Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Client Context Menu can be reached by right-clicking in the client area, outside the context of one of the editing windows.  It has items related to the client area such as tiling and closing/saving files.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc cmclient.png\}\cf2 
\par \pard\f1 
\par \b\f0 Close\b0  closes the currently selected window.
\par 
\par \b Close All\b0  closes all edit windows on the client area, but not special windows that have been undocked.
\par 
\par \b Save All \b0 saves the contents of all open windows.
\par \b 
\par Create New View \b0 makes a copy of an existing editor window.  The new window will be a new view that looks at the same data as the existing view.  Changes in one view will be reflected in the other.  This mechanism allows one to view two separate locations in the same file.
\par 
\par \b Cascade\b0  orders all client area windows so that they are the same size and are overlapped in a staggered fashion, where the title bar of each successive window is just below and to the right of the previous window.
\par 
\par \b Tile Horizontally\b0  orders all client area windows so that they are stacked from the top of the client area to the bottom
\par 
\par \b Tile Vertically\b0  orders all client area windows so that they are stacked from the left of the client area to the right.
\par 
\par \b Arrange Icons\b0  organizes any client area windows that have been iconized.
\par \f1 
\par \b\f0 More Windows...\b0  opens the \cf4\strike Select Window Dialog\cf3\strike0\{linkID=330\}\cf2  to allow a window to be selected.\f1 
\par }
210
Scribble210
Workarea Context Menu
context;dependencies;menu;workspace context menu;



Writing



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Workarea Context Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Workarea Context Menu may be reached by right-clicking in the \cf3\strike WorkArea Window\cf4\strike0\{linkID=190\}\cf0  on the name of the workarea.  \cf2 It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc cmworkarea.png\}\cf2 
\par \pard 
\par 
\par \b New Project\b0  brings up the \cf3\strike New Project\cf4\strike0\{linkID=360\}\cf2  dialog for selecting the characteristics of a new project.
\par 
\par \b Existing Project\b0  brings up the \cf3\strike Open File\cf4\strike0\{linkID=350\}\cf2  dialog for importing an existing project.
\par 
\par \b Make\b0  calculates which files have changed, and recompiles those files as well as recreates any files which depend on them\b 
\par 
\par Make Active Project \b0 performs a make, but only on the active project.\b 
\par 
\par ReBuild All\b0  rebuilds all projects in the workarea by removing all output files then doing a \b make\b0 .\b 
\par 
\par Stop Build \b0 stops a build which is in progress.\b 
\par 
\par Run/Continue \b0 starts the debugger if it isn't running, at the active project.  \cf3\strike Debugging Windows\cf4\strike0\{linkID=320\}\cf2  are opened at this point.  The program will stop at the first break point it encounters, or at the start of the \i main()\i0  or \i WinMain() \i0 function if no breakpoints exist.\b 
\par 
\par Close Work Area \b0 closes the workarea.
\par 
\par \b Project Dependencies\b0  brings up the \cf3\strike Project Dependencies dialog\cf4\strike0\{linkID=582\}\cf2  to allow selection of which projects depend on other projects.
\par 
\par \b Select Profile\b0  opens the \cf3\strike Select Profile dialog\cf4\strike0\{linkID=589\}\cf2  to allow selection of the current \cf3\strike profile\cf4\strike0\{linkID=585\}\cf2\b 
\par 
\par Properties \b0 opens the \cf3\strike Project Properties Dialog\cf4\strike0\{linkID=580\}\cf2\f1 
\par }
220
Scribble220
Folder Context Menu




Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Folder Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The folder context menu iss accessible by right-clicking on a folder in the \cf2\strike Workarea Window\cf3\strike0\{linkID=190\}\cf0 .  has items related to creating and removing folders within a project.  \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\qc\cf3\{bmc cmfolder.png\}\cf0 
\par \pard 
\par \b New File\b0  opens an \cf2\strike Open File\cf3\strike0\{linkID=350\}\cf0  dialog to allow entering the name of a new file which is being created as it is added to the project.
\par 
\par \b Existing File\b0  opens an Open File dialog to allow opening an existing file to add it to the project.
\par 
\par \b New Folder\b0  creates a new folder underneath the current one.
\par 
\par \b Remove\b0  removes a folder.  Any files in the folder will be moved to the parent of the removed folder.
\par 
\par \b Rename\b0  allows renaming the folder.
\par 
\par \f1 
\par }
230
Scribble230
Project Context Menu
dependencies;



Writing



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Project Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The Project Context Menu iss accessible by right-clicking on a project name in the \cf2\strike Workarea Menu\cf3\strike0\{linkID=70\}\cf0 .  It has menu items related to the project.  \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\qc\cf3\{bmc cmproject.png\}\cf0 
\par \pard\f1 
\par \b\f0 New File\b0  opens a \cf2\strike New File Dialog\cf3\strike0\{linkID=365\}\cf0  dialog to allow entering the name of a new file which is being created as it is added to the project.
\par 
\par \b Existing File\b0  opens an Open File dialog to allow opening an existing file to add it to the project.
\par 
\par \b New Folder\b0  creates a new folder underneath the current one.
\par 
\par \cf4\b Make\b0  calculates which files have changed, and recompiles those files as well as recreates any files which depend on them\b 
\par 
\par ReBuild \b0 rebuilds all projects in the workarea by removing all output files then doing a \b make\b0 .\b 
\par 
\par Stop Build \b0 stops a build which is in progress.\b 
\par \cf0\b0 
\par \cf4\b Run/Continue \b0 starts the debugger if it isn't running, at the active project.  \cf2\strike Debugging Windows\cf3\strike0\{linkID=320\}\cf4  are opened at this point.  The program will stop at the first break point it encounters, or at the start of the \i main()\i0  or \i WinMain() \i0 function if no breakpoints exist.\b 
\par 
\par \cf0 Run Without Debugger \b0 starts the program as a separate executable, without debugging.  If it is a console program, a separate console window will be created for it to display data in.\b 
\par \b0 
\par \b Remove\b0  removes a project from the workarea.  The project and all its files will still reside on the file system.
\par 
\par \b Rename\b0  allows renaming the project.
\par 
\par \b Build Window\b0  is an alternate way to build the project that performs the build using a make file, in a separate console window.  Errors shown in this window will not be readily accessible for locating corresponding lines in source files. 
\par 
\par \cf4\b Project Dependencies\b0  brings up the \cf2\strike Project Dependencies dialog\cf3\strike0\{linkID=582\}\cf4  to allow selection of which projects depend on other projects.
\par 
\par \cf0\b Set As Active Project\b0  sets the current project as the active project, which selects it as the default project the debugger will use.
\par 
\par \b Properties\b0  opens the project properties window\f1 
\par }
240
Scribble240
File Context Menu




Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 File Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The File Context Menu may be reached by right clicking on an individual source file in the \cf2\strike Workarea Window\cf3\strike0\{linkID=70\}\cf0 .  Ithas menu items related to individual files within a project.  \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\qc\cf3\{bmc cmfile.png\}\cf0 
\par \pard\f1 
\par \b\f0 Open\b0  opens an\cf2\strike  Edit Window\cf3\strike0\{linkID=50\}\cf0  with the contents of the file.  In the case of a resource, it opens the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0  for the file, it is then necessary to click on a resource to edit it.
\par 
\par \b Compile File\b0  uses the appropriate tool to compile, assemble, or otherwise process the file to create an output file.
\par 
\par \b Remove\b0  removes a file from the project.  The file will still reside on the file system.
\par 
\par \b Rename\b0  allows renaming the file.
\par 
\par \b Properties\b0  opens a property window for editing build-specific properties for the file.
\par 
\par \f1 
\par }
250
Scribble250
Build Menu
build;build menu;make;menu;



Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Build Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Build Menu has menu items related to compiling and linking programs.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc mbuild.png\}\cf2 
\par \pard 
\par \b Compile File\b0  compiles the file in the currently active editor window.\b 
\par 
\par Make\b0  calculates which files have changed, and recompiles those files as well as recreates any files which depend on them\b 
\par 
\par Make Active Project \b0 performs a make, but only on the active project.\b 
\par 
\par ReBuild All\b0  rebuilds all projects in the workarea by removing all output files then doing a \b make\b0 .\b 
\par 
\par Stop Build \b0 stops a build which is in progress.\b 
\par 
\par Generate Make File \b0 creates a \cf4\strike Generated Make File\cf3\strike0\{linkID=270\}\cf2 .
\par 
\par \b 
\par \f1 
\par }
260
Scribble260
Help Menu
help;help menu;menu;



Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Help Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Help Menu has menu items related to documentation.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc mhelp.png\}\cf2 
\par \pard 
\par \b IDE Help \b0 shows the \cf4\strike index page\cf3\strike0\{linkID=10\}\cf2  of this help file.\b 
\par 
\par Language Help\b0  shows help about the \cf4\strike C Language\cf3\strike0\{linkFile=chelp.chm\}\cf2 .
\par \b 
\par RTL Help \b0 shows help specific to the \cf4\strike C Run-time Library\cf3\strike0\{linkFile=crtl.chm\}\cf2 .
\par \b 
\par Tools Help\b0  shows help specific to the \cf4\strike Command Line Tools\cf3\strike0\{linkFile=TOOLS.chm\}\cf2 , and help related to compiler extensions used by the C compiler in this package.
\par 
\par \b WIN32 Help\b0  opens the selected \cf4\strike WIN32 help\cf3\strike0\{linkID=555\}\cf2  system to its main page.\b 
\par 
\par Getting Started \b0 shows general information about how to get started with creating a project in this Ide
\par 
\par \b Configure WIN32 Library \b0 downloads the WIN32 offline help library, when the help mode is set to Windows 8
\par \b 
\par About \b0 shows the version information of the IDE.
\par \b\f1 
\par }
270
Scribble270
Generated Make File
file;generated makefile;make;



Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Generated Make File\cf2\b0\f1\fs20 
\par 
\par \f0 The IDE is capable of converting a workarea into a make file compatible with the OMAKE command line make utility.   This allows exporting the build steps, so that the file can be built from the command line instead of the IDE.
\par \f1 
\par \f0 The generation is selected through the \b Generate Make File\b0  option on the \cf3\strike Build Menu\cf4\strike0\{linkID=250\}\cf2 . If the workarea has multiple project, the make file will contain scripts suitable for rebuilding all projects.
\par \f1 
\par \f0 The paths in the make file are made relative to the location of the make file.  This means that if projects are moved to a new location, they must be kept in the same relative location to each other and to the make file.\f1 
\par }
280
IDM_INFORMATION_WINDOW
Information Window
debugging windows;information window;special windows;windows;



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red255\green0\blue0;}
\viewkind4\uc1\pard\ri1440\cf1\b\fs32 Information Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Information window is accessible by clicking \b Information Window\b0  on the \cf2\strike View Menu\cf3\strike0\{linkID=180\}\cf0 .  It has a combo box to select what type of information to view.  
\par 
\par \pard\ri1440\qc\cf3\{bmc winfo.png\}\cf0 
\par \pard\ri1440 
\par In this example, we see the IDE creating the output files for a project.
\par 
\par The combobox  will generally auto-select itself when information is being displayed in them, or they can be manually selected at any time.
\par 
\par The various icons at the top of the window allow navigation and searching in the text.
\par 
\par The \b Build\b0  tab gives information about the compile in progress.  Errors and general status can be viewed here.  Clicking on a line that shows an error or warning will open an edit window and position the file to that line.  In the build tab, errors will be in \cf4 red\cf0  and warnings will be in \cf1 blue\cf0 .  
\par 
\par The \b Debug\b0  tab gives information about the debug status, such as what DLLs and THREADs are starting or ending.  It also displays messages from the program that are sent via the OutputDebugString API function.  These messages will be in \cf1 blue\cf0 .
\par 
\par Right-Clicking on the information window pane opens the \cf2\strike Information Context Menu\cf3\strike0\{linkID=290\}\cf0 .
\par }
290
Scribble290
Information Context Menu
context;information context  menu;menu;



Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Information Context Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Information Context Menu is accessed by right-clicking in the \cf3\strike Information Window\cf4\strike0\{linkID=280\}\cf2 . It has menu items to aid in rebuilding the current project.  It appears as follows:
\par 
\par \pard\qc\cf4\{bmc cminfo.png\}
\par \pard 
\par \cf2\b Make\b0  calculates which files have changed, and recompiles those files as well as recreates any files which depend on them\b 
\par 
\par Make Active Project \b0 performs a make, but only on the active project.\b 
\par 
\par ReBuild All\b0  rebuilds all projects in the workarea by removing all output files then doing a \b make\b0 .\b 
\par 
\par Stop Build \b0 stops a build which is in progress.\b 
\par 
\par }
295
Scribble295
Error List
error list;windows;



Writing



FALSE
14
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Error List\cf0\lang1049\b0\f1\fs20 
\par \pard\ri1440\lang1033 
\par \f0 The Error List is accessible by clicking \b Error List\b0  on the \cf2\strike View Menu\cf3\strike0\{linkID=180\}\cf0 .  This window shows a list of errors and warnings.   It appears as follows:
\par 
\par \pard\ri1440\qc\cf3\{bmc werror.png\}\cf0 
\par \pard\ri1440 
\par In this example, we see several warnings (yellow icon).   If there were an error, the icon next to it would show a red circle with an X.
\par 
\par The buttons at the top of the window allow configuration of whether to view errors, warnings, or both.   Clicking on an error line brings up an \cf2\strike Editor Window\cf3\strike0\{linkID=50\}\cf0  to view the line with the problem. 
\par 
\par The contents of this window are derived from the \cf2\strike Information Window's\cf3\strike0\{linkID=280\}\cf0  listing of build results, however, the Information Window does not have to be displayed for this window to function.
\par }
300
Scribble300
Integrated Debugger
debug;integrated debugger;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1440\cf1\b\fs32 Integrated Debugger\cf0\b0\f1\fs20 
\par 
\par \f0 The debugger is started by using \b Start Debugging\b0  on the \cf2\strike Debug Menu\cf3\strike0\{linkID=310\}\cf0 .
\par \f1 
\par \f0 The debugger is a full-featured debugger.  Providing that the executable is compiled to enable debug information, all features are available once the debugger has started.  Note that breakpoints may be set while the debugger is stopped; this allows the possibility of setting breakpoints before the program starts.
\par 
\par When the debugger is started the program will start, and run until it hits a break point.  Alternately, press F5 or F10.  If F5 is pressed, the program will start running and run until it either ends or hits a breakpoint.  If F10 is pressed the program will start running and stop at the beginning of \i main()\i0  or \i WinMain()\i0 .
\par 
\par Following is an example of an editor window when the program is stopped.
\par 
\par \pard\ri1440\qc\cf3\{bmc debugsession.png\}\cf0 
\par \pard\ri1440 
\par The dark grey lines at the left indicate which lines of the program generated executable code.  The red circle is a \cf2\strike Break point\cf3\strike0\{linkID=410\}\cf0 .  The yellow arrow indicates at what point the program has stopped.
\par 
\par Note that when the debugger stops on a function declaration, the processor stack is not set up yet.  Therefore, it is not possible to accurately determine where the function's variables are.  Local variables will appear as 'out of scope'.  Single stepping past the function prototype will cause the processor stack to be initialized.
\par 
\par There are several menu items on the \cf2\strike Editor Context Menu\cf3\strike0\{linkID=150\}\cf0  that relate to debugging.  There is also a \cf2\strike Debug Toolbar\cf3\strike0\{linkID=760\}\cf0  with some commonly used debugging functions such as starting the program, and single stepping.    An additional button on the Debug Toolbar allows the possibility of stepping out of a function.
\par 
\par Single stepping can also be performed by pressing F10 for step over, and F11 for step into.  F9 toggles breakpoints. F5 starts the program running when it is stopped.
\par 
\par The \cf2\strike Project Debug Properties\cf3\strike0\{linkID=570\}\cf0  holds settings used for setting up debugging on a project by project basis.  the \cf2\strike Hints\cf3\strike0\{linkID=530\}\cf0  properties page has settings for enabling and disabling debug hints on a global basis.
\par 
\par Only one project can be started at a time, however if some projects are DLLs which are called by the active project, the debugger will allow symbolic debugging of them.\f1 
\par }
310
Scribble310
Debug Menu
debug;debug menu;menu;



Writing



FALSE
59
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Debug Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Debug Menu has menu items related to debugging the project.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc mdebug.png\}\cf2 
\par \pard\f1 
\par \b\f0 Run/Continue \b0 starts the debugger if it isn't running, at the active project.  \cf4\strike Debugging Windows\cf3\strike0\{linkID=320\}\cf2  are opened at this point.  The program will stop at the first break point it encounters, or at the start of the \i main()\i0  or \i WinMain() \i0 function if no breakpoints exist.   If the debugger is stopped in the bugger program execution is resumed.\b 
\par 
\par Stop Debugging\b0  stops the debugger and closes Debugging Windows.\b 
\par 
\par \pard\ri1440 Run To Cursor\b0  \cf0 runs the program.  It will stop at the next break point encountered, when the program exits, or when the program flow reaches the cursor position in the current active edit window.
\par 
\par \b Step Over\b0  steps over the current statement.   If the current statement is a function call the entire function is run; breakpoints in the function may still be hit though.
\par 
\par \b Step In\b0  steps into a function, if the current statement has one.   Otherwise it steps over the statement.
\par 
\par \b Step Out\b0  runs until the current function exits, or until a breakpoint is hit.
\par \pard\cf2\b 
\par Stop\b0  stops the program if it is running.\b 
\par 
\par \cf0 Run Without Debugger \b0 starts the program as a separate executable, without debugging.  If it is a console program, a separate console window will be created for it to display data in.\b 
\par \cf2 
\par Add To Watch\b0  opens the \cf4\strike Add To Watch Dialog\cf3\strike0\{linkID=370\}\cf2 .  This allows an \cf4\strike expression\cf3\strike0\{linkID=800\}\cf2  to be added to the watch window.\b 
\par 
\par Hardware Breakpoints\b0  opens the \cf4\strike Hardware Breakpoints\cf3\strike0\{linkID=730\}\cf2  dialog.\b 
\par 
\par \pard\ri1440\tx660\tx2820\cf0 Data Breakpoints\b0  opens the \cf4\strike Data Breakpoint\cf3\strike0\{linkID=890\}\cf0  dialog to allow adding a data breakpoint on the word under the cursor.
\par 
\par \b Add Function Breakpoint\b0  prompts for a function name, then sets a breakpoint there.'
\par 
\par \b Toggle Breakpoint\b0  toggles the break point setting for the current line.  See \cf4\strike Breakpoints\cf3\strike0\{linkID=410\}\cf0  for further information about breakpoints.
\par 
\par \pard\cf2\b Remove All Breakpoints\b0  removes all breakpoints from the break point list.
\par \b\f1 
\par \f0 Disable All Breakpoints\b0  disables all active breakpoints, but leaves the lines marked as breakpoints.
\par \b\f1 
\par \pard\ri1440\tx660\tx2820\cf0\f0 Return To Origin\b0  opens the window the current breakpoint is associated with and navigates to the breakpoint line.\cf2 
\par \pard 
\par The following menu exist on the \cf0\b Debug Windows\cf2\b0  submenu.  These items allow selection of various debug related windows, they are grayed out until the \cf4\strike Integrated Debugger\cf3\strike0\{linkID=300\}\cf2  is started.
\par 
\par \b Breakpoints\b0  opens or closes the \cf4\strike Breakpoints Window\cf3\strike0\{linkID=415\}\cf2 .   When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Disassembly\b0  opens or closes the \cf4\strike Assembly Window\cf3\strike0\{linkID=670\}\cf2 .  When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Memory\b0  opens or closes one of four \cf4\strike Memory Windows\cf3\strike0\{linkID=680\}\cf2 .  When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Register\b0  opens or closes the \cf4\strike Register Window\cf3\strike0\{linkID=720\}\cf2 .  When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Call Stack\b0  opens or closes the \cf4\strike Call Stack Window\cf3\strike0\{linkID=700\}\cf2 .  When the window is displayed a check mark will appear next to the menu item.
\par 
\par \b Thread \b0 opens or closes the \cf4\strike Thread Window\cf3\strike0\{linkID=710\}\cf0 .  When the window is displayed a check mark will appear next to the menu item.
\par \cf3 
\par \cf0\b Watch \b0 opens or closes one of four \cf4\strike Watch Windows\cf3\strike0\{linkID=650\}\cf2 .  When the window is displayed a check mark will appear next to the menu item.\f1 
\par \b 
\par \f0 Locals \b0 opens or closes the \cf4\strike Locals Window\cf3\{\strike0 linkID=655\}\cf2 .   When the window is displayed a check mark will appear next to the menu item.
\par \b\f1 
\par }
320
Scribble320
Debugging Windows
debug;debugging windows;windows;



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1440\cf1\b\fs32 Debugging Windows\cf0\b0\f1\fs20 
\par 
\par \f0 The IDE has several special windows used for helping to debug.  These windows are available from the \cf2\strike Debug Windows Menu\cf3\strike0\{linkID=310\}\cf0  when the debugger is running.
\par 
\par When the debugger is stopped, the IDE will close all debug windows.  However, it will remember which ones were open  and their position, and reopen them the next time the debugger is started.
\par 
\par The debugging windows are as follows:
\par 
\par \pard\ri1440\tx660\tx4200\tab\cf2\strike Disassembly Window\cf3\strike0\{linkID=670\}\cf0\tab Shows the assembly language associated with the program
\par \tab\cf2\strike Breakpoints Window\cf3\strike0\{linkID=415\}\tab\cf0 Shows all current breakpoints
\par \tab\cf2\strike Memory Window\cf3\strike0\{linkID=680\}\cf0\tab Shows a memory dump
\par \tab\cf2\strike Register Window\cf3\strike0\{linkID=720\}\tab\cf0 Shows registers
\par \tab\cf2\strike Call Stack Window\cf3\strike0\{linkID=700\}\cf0\tab Shows a back trace of the processor stack
\par \tab\cf2\strike Threads Window\cf3\strike0\{linkID=710\}\cf0\tab Shows a list of threads
\par \tab\cf2\strike Watch Window\cf3\strike0\{linkID=650\}\cf0\tab Shows the value of variables
\par \tab\cf2\strike Locals Window\cf3\strike0\{linkID=655\}\cf0\tab Shows the values of local(auto) variables that are currently in scope
\par \pard\ri1440\tx680\tx3680 
\par }
330
Scribble330
Select Window Dialog
dialog;select window dialog;



Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Select Window Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Select Window Dialog is accessible from the \i More Windows...\i0  item of the \cf3\strike Window Menu\cf4\strike0\{linkID=140\} \cf0 when there are enough edit windows open in the client area that the list of open windows will not fit on the Window Menu.  It is also available from the \i More Windows...\i0  item of the \cf3\strike Client Context Menu\cf4\strike0\{linkID=195\}\cf0 . It shows a list of open windows and appears similar to the following:
\par 
\par \pard\qc\cf4\{bmc morewindows.png\}
\par \pard 
\par \cf0 Double Clicking on the name of a file name will cause that window to come to the top of the view area.  
\par 
\par Special Windows such as the Watch Window will also appear in the client area if undocked, and will show up in this list.
\par 
\par \cf2\f1 
\par }
340
Scribble340
Goto Line Dialog
dialog;goto line dialog;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Goto Line Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Goto Line Dialog can be reached from the \cf3\strike Navigation Toolbar\cf4\strike0\{linkID=770\}\cf2  or from the \b Goto Line\b0  option of the \cf3\strike View Menu\cf4\strike0\{linkID=180\}\cf2 .  It allows a line number to be entered.  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc goto.png\}\cf2 
\par \pard 
\par When \b Ok\b0  is clicked, the currently active editor window will be positioned to the specified line.  
\par 
\par Note that it is possible to have line numbers displayed to the left of each line by selecting the \b Show Line Numbers \b0 of the \cf3\strike Basic Editor Settings\cf4\strike0\{linkID=490\}\cf2  property page.\f1 
\par }
350
Scribble350
Open File Dialog
dialog;file open;file save;new;new file;new target;new workspace;open file dialog;open target;open workspace;



Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Open File Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Open File Dialog is a standard windows dialog used for selecting files.  It is used to open files to edit, open workareas, locate projects to add to the workarea, and so forth.  In some cases such as opening files or adding them to a project, the open file dialog will accept a selection of multiple files.
\par 
\par The Open File Dialog appears as follows
\par 
\par \pard\qc\cf3\{bmc openfile.png\}\cf2 
\par \pard 
\par The enhanced Recent Directories combo box holds a list of directories that have been visited recently.  It may be used as a convenient method for navigating to recently used directories.
\par 
\par The title bar changes to reflect the operation being performed.  In this example, one or more text files may be opened.
\par 
\par }
360
IDH_NEW_PROJECT_DIALOG
New Project Dialog




Writing



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 New Project Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The New Project Dialog may be reached from the \cf2\strike Workarea Context Menu\cf3\strike0\{linkID=210\}\cf0 , or from the \b WorkArea->Add New Project\b0  item of the \cf2\strike File Menu\cf3\strike0\{linkID=60\}\cf0 .  It allows selection of the name and location of a new project, and its type.  The project type controls what kind of output file will be created, for example an executable file or a library. \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\qc\cf3\{bmc newproject.png\}
\par \pard 
\par \cf0\f1 
\par \f0 To create a new project, first select its type.  The following types are available:
\par 
\par \pard\fi-3100\li4180\tx4200\b Console\b0\tab a console application, the type of command line program standard C runs as.  Can also include windowing functions.\b 
\par Windows GUI\b0\tab a program with a windows GUI\b 
\par Windows DLL\b0\tab shared code and data in an executable library format of the type that Windows 32 bit requires\b 
\par Static Library\b0\tab shared code and data in a library format that can be combined with other code when creating programs.  
\par \tab This is the type of library often found in non-windows environments.
\par 
\par \pard Once the type has been chosen, select a name and location for the project.  This name does not need to have an operating system extension; an extension will be added automatically based on the file type.  This IDE will not automatically create the location, so it is necessary to select an existing directory.  Pressing the button to the right of the location will bring up a dialog that allows searching for a directory.
\par 
\par Selecting \b Create New Directory For Project \b0 will result in the IDE creating a subdirectory with the projects name to put the project files in.\b\f1 
\par }
362
IDH_NEW_WORKAREA_DIALOG
New Work Area Dialog




Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 New Work Area Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The New Work Area Dialog is accessible by selecting \b WorkArea->New\b0  from the \cf2\strike File Menu\cf3\strike0\{linkID=60\}\cf0 .  It allows selection of the name and location of a new workarea.  \cf4 It appears similar to what follows:\cf0 
\par 
\par \pard\qc\cf3\{bmc newworkarea.png\}
\par \pard 
\par \cf0\f1 
\par \f0 In this dialog, select a name and location for the project.  This name does not need to have an operating system extension; an extension will be added automatically based on the file type.  This IDE will not automatically create the location, so it is necessary to select an existing directory.  Pressing the button to the right of the location will bring up a dialog that allows searching for a directory.
\par 
\par When typing the file name, the IDE will automatically attempt to update the folder name by adding a subdirectory name based on the file name.  However, this can be edited after the filename is typed in to put the workarea in an arbitrary folder.
\par 
\par }
365
IDH_NEW_FILE_DIALOG
New File Dialog




Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 New File Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The New File Dialog is accessible from the Project Context Menu, and by selecting \b File->New\b0  from the \cf2\strike File Menu\cf3\strike0\{linkID=60\}\cf0 . allows selection of the name and location of a new file, and its type.  The file type controls what extension the file will get, and thus how the file will be processed.  For example C language files will be compiled with the C compiler.
\par 
\par The file type may also cause the file to be automatically sorted into a default folder, 
\par \cf4 
\par The New File Dialog appears similar to what follows:
\par \cf0 
\par \pard\qc\cf3\{bmc newfile.png\}
\par \pard\cf0\f1 
\par \f0 To create a new file, first select its type.  The following types are available:
\par 
\par \pard\fi-3100\li4180\tx4200\b C Program File\b0\tab Program source code(.C) to be compiled with the compiler compiled with OCC
\par \b Header File\b0\tab Header Information(.H) for program source code included in other files
\par \b Resource File\b0\tab Window Resource (.RC) file compiled with ORC
\par \b Module Definition File\b0\tab Module Definition (.DEF) file sometimes used in creating windows programs
\par \b Assembly Language File\b0\tab Assembly Language source code (.asm) assembled with OASM
\par \b Text File\b0\tab Plain Text (.txt) that isn't compiled\b 
\par \b0\tab This is the type of library often found in non-windows environments.
\par 
\par \pard Once the type has been chosen, select a name for the file.  This name does not need to have an operating system extension; an extension will be added automatically based on the file type.  The IDE will place the new file in the project when it is first saved.
\par 
\par }
370
Scribble370
Add To Watch Dialog
add to watch dialog;dialog;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Add To Watch Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Add To Watch Dialog is accessible from the\cf3\strike  Editor Context Menu\cf4\strike0\{linkID=150\}\cf2 , and from the \cf3\strike Watch Window Context Menu\cf4\strike0\{linkID=660\}\cf2 .  It allows entry of the name of a variable.  It appears similar to what follows:
\par \pard\qc 
\par \cf4\{bmc addwatch.png\}
\par 
\par \pard\cf2 Any \cf3\strike expression\cf4\strike0\{linkID=800\}\cf2  may be entered into the text field.  The combo box remembers recently selected expressions.
\par 
\par When \b Ok\b0  is pressed, the variable will appear in the \cf3\strike Watch Window\cf4\strike0\{linkID=650\}\cf2  along with its value.\f1 
\par }
380
Scribble380
Browse To Dialog
browse;browse to dialog;



Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Browse To Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Browse To Dialog is accessible from the \cf3\strike View->Browse Menu\cf4\strike0\{linkID=170\}\cf2 .  It allows entry of a variable or function name to locate.  It appears similar to what follows:\f1 
\par 
\par \pard\qc\cf4\{bmc browseto.\f0 png\f1\}\cf2 
\par \pard 
\par \f0 After pressing Ok, the definition of the name is located and shown in an \cf3\strike Editor Window\cf4\strike0\{linkID=50\}\cf2 .\f1 
\par }
390
Scribble390
Bookmark Dialog
bookmark dialog;bookmarks;



Writing



FALSE
11
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Bookmark Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Bookmark Dialog is accessed from the \cf3\strike View->Bookmarks Menu\cf4\strike0\{linkID=160\}\cf2 .  It offers a way to select from a list of bookmarks.  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc showbookmarks.png\}\cf2 
\par \pard 
\par Double-Clicking on a bookmark opens an \cf3\strike Editor Window\cf4\strike0\{linkID=50\}\cf0  which shows the line the bookmark is on.\cf2\f1 
\par 
\par }
400
Scribble400
Bookmarks
bookmarks;



Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1500\cf1\b\fs32 Bookmarks\cf0\b0\f1\fs20 
\par 
\par \f0 Bookmarks are used to mark positions in the text for easy traversal.  They may be turned on and off for any line in any files.  A visual indication is given to the left of the editor window as to which lines have bookmarks enabled.
\par 
\par The \cf2\strike View Bookmarks Menu\cf3\strike0\{linkID=160\}\cf0  has various items which can be used to turn bookmarks on and off, and traverse through the list of bookmarks.  The \cf2\strike Bookmark Toolbar\cf3\strike0\{linkID=780\}\cf0  has the same items.  Bookmarks may also be toggled via the \cf2\strike Editor Context Menu\cf3\strike0\{linkID=150\}\cf0  The \b Show Bookmarks \b0 item located in some of these locations brings up the \cf2\strike Bookmark Dialog\cf3\strike0\{linkID=390\}\cf0 , which can be used to select an arbitrary bookmark from the list.
\par 
\par 
\par }
410
Scribble410
Breakpoints
breakpoints;



Writing



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Breakpoints\cf2\b0\f1\fs20 
\par 
\par \f0 Breakpoints are set in order to bring the program to a stop.  This is useful because variables cannot be analyzed while the program is running.  Typical things to do when a break point is triggered are to look at variables in the \cf3\strike Watch Window\cf4\strike0\{linkID=650\}\cf2  or by moving the mouse over the variable name, analyze the \cf3\strike call stack\cf4\strike0\{linkID=700\}\cf2  back trace, or look at the list of running \cf3\strike threads\cf4\strike0\{linkID=710\}\cf2 .
\par 
\par Breakpoints can be set in a variety of ways, for example from the \cf3\strike Debug Toolbar\cf4\strike0\{linkID=760\}\cf2 , from the \cf3\strike Debug Menu\cf4\strike0\{linkID=310\}\cf2 ,
\par from the \cf3\strike Editor Context Menu\cf4\strike0\{linkID=150\}\cf2 , or by selecting a line in an editor window and pressing F9.
\par 
\par Breakpoints may be set while the \cf3\strike Integrated Debugger\cf4\strike0\{linkID=300\}\cf2  is stopped, but only take effect when the debugger is running.  
\par 
\par Breakpoints may also be set while in the \cf3\strike Disassembly Window\cf4\strike0\{linkID=670\}\cf2 .   In this case a breakpoint can be set on arbitrary machine instructions, not just at the beginning of a line.
\par 
\par A \cf3\strike Breakpoint Window\cf4\strike0\{linkID=415\}\cf2  can show a list of current breakpoints and allow individually enabling or disabling them.
\par 
\par When no breakpoints are set, the program stops at the \i main()\i0  or \i Winmain()\i0  function when run, as if a breakpoint had been set there.
\par 
\par Another type of break point is implicitly set when the \b Run To\b0  item on the Debug Toolbar is used.  A break point is set at the cursor address to force the program to stop there.
\par 
\par \pard\ri1500\cf0 A breakpoint may also be set with the mouse when the debugger is running.  Simply double click in the area on the left of the \cf3\strike Editor Window\cf4\strike0\{linkID=50\}\cf0 .
\par 
\par \pard\cf2 The debugger supports \cf3\strike Hardware Breakpoints\cf4\strike0\{linkID=730\}\cf0 . This type of break point is supported by the microprocessor itself, and is used to track reads and writes to variables.  It is a simpler but faster form of tracking changes to variables, useful for small quantities such as single characters or integers.
\par 
\par Finally, the debugger supports \cf3\strike Data Breakpoints\cf4\strike0\{linkID=890\}\cf0 .  This type of breakpoint detects when a variable changes.  It can handle more complex variables than the corresponding Hardware Breakpoints, such as entire structures or arrays.  However, it uses a form of detection that has the potential to slow the program down somewhat while detection of changes is being done.
\par 
\par \cf2\f1 
\par }
415
Scribble415
Breakpoints Window
breakpoints;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Breakpoints Window\cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 The Breakpoints window allows viewing and editing a list of breakpoints.   It looks similar to what follows:
\par 
\par 
\par \pard\qc\cf2\{bmc wbreakpoint.png\}
\par \pard\cf0 
\par This example has two breakpoints; one will stop the program and the other is currently disabled.   This window also shows data and hardware breakpoints, if they have been selected.
\par 
\par Right clicking in this window brings up the \cf3\strike Breakpoint Context Menu\cf2\strike0\{linkID=417\}\cf0 .
\par 
\par Clicking on a code breakpoint takes one to the line the breakpoint is defined on.   Clicking on data or hardware breakpoints has no effect.
\par \lang1049\f1 
\par }
417
Scribble417
Breakpoint Context Menu




Writing



FALSE
20
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Breakpoint Context Menu\cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 The breakpoint context menu has several items that may be used to modify the contents of the \cf2\strike Breakpoints Window\cf3\strike0\{linkID=415\}.
\par 
\par \pard\qc\{bmc cmbreakpoint.png\}
\par \pard\cf0 
\par \b Hardware Breakpoints\b0  allows creating a \cf2\strike Hardware Breakpoint\cf3\strike0\{linkID=730\}\cf0 
\par 
\par \b Data Breakpoints\b0  allows creating a \cf2\strike Data Breakpoint\cf3\strike0\{linkID=890\}\cf0 
\par 
\par \b Add Function Breakpoint\b0  allows creating a breakpoint at the beginning of a function
\par 
\par \b Remove This Breakpoint\b0  removes the breakpoint that was right-clicked on from the list of breakpoints
\par 
\par \b Remove All Breakpoints\b0  remove all breakpoints removes all breakpoints from the list
\par 
\par \b Enable All Breakpoints\b0  enables or disables all breakpoints in the list\lang1049\f1 
\par }
420
Scribble420
Integrated Platform Help




Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\tx8520\cf1\b\fs32 Integrated Platform Help\cf0\b0\f1\fs20 
\par 
\par \f0 The IDE has four help files.  This is the first one, which is accessed by pressing F1 or by using the help buttons arranged on various dialogs.  The second one is the \cf2\strike Tools Help File\cf3\strike0\{linkFile=tools.chm\}\cf0 , which gives specific information about the command line tools the IDE utilizes to create programs.  The third one is the \cf2\strike RTL Help File\cf3\strike0\{linkFile=crtl.chm\}, \cf0 which gives specific information about run-time functions available in the C language.  The fourth one is the \cf2\strike Language Help File\cf3\strike0\{linkFile=chelp.chm\}\cf0 
\par 
\par These help files are all accessible from the \cf2\strike Help Menu\cf3\strike0\{linkID=260\}\cf0 .
\par 
\par Run Time Library help is also available for the word under the cursor by pressing SHIFT-F1.
\par 
\par 
\par }
430
Scribble430
Browse Information
browse;browse information;



Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Browse Information\cf0\b0\f1\fs20 
\par 
\par \f0 The IDE offers source code browsing.  Placing a cursor on a variable or structure name, the selecting \b Browse to Definiton or Browse to Declaration \b0 on the \cf2\strike View Browse Menu\cf3\strike0\{linkID=170\}\cf0  will browse to the variable or structure declaration.  Alternatively the Browse To selection will open the \cf2\strike Browse To Dialog\cf3\strike0\{linkID=380\}\cf0  and allow a name to be entered directly.  
\par 
\par The browse function requires compiler support and must be enabled from the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  After enabling it, the project must be recompiled with the Build All option.   In C++ code it is also possible to browse to a function which has been referenced through a structure instance, but this requires that \cf2\strike Code Completion\cf3\strike0\{linkID=530\}\cf0  also be enabled.
\par 
\par Various C++ language constructructs are supported, for example the namespace selector.
\par 
\par Browsing is only possible within C language files and headers.  Note that the browse information file (which is handled in the background) may become very large for large programs.
\par 
\par Browse information is also used by the \cf2\strike Jump List\cf3\strike0\{linkID=900\} \cf0 and the \cf2\strike Variable Usages Window\cf3\strike0\{linkID=435\}.\cf0 
\par 
\par Browsing is enabled in the \cf2\strike Hints Configuration\cf3\strike0\{linkID=530\}\cf0 .
\par 
\par If a browse is requested, and browsing is not enabled, the IDE will ask if the workarea should be rebuilt with browsing enabled.  If the answer is yes, the workarea will be rebuilt as if by the \b Rebuild All \b0 selection on the \cf2\strike Build Menu\cf3\strike0\{linkID=250\}\cf0 , and then the browse will complete.
\par }
435
Scribble435
Variable Usages Window
variable;variable usage;



Writing



FALSE
14
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Variable Usages Window
\par 
\par \cf0\b0\fs20 The Variable Usages window is used to view the position of the definition and usages of a variable.   It looks similar to what follows:
\par \cf1\b\fs32 
\par \pard\qc\cf2\b0\fs20\{bmc wvarusage.png\}\cf0\lang1049\f1 
\par \pard 
\par 
\par \lang1033\f0 On this window, type the name of the variable in the combo box.   The browse information will be used to determine where the variable is defined (shown with the green plus) and where it is used (shown with the gray box).   It is also possible to see the location where the variable is declared, which will be indicated by a yellow triangle with an exclamation point.
\par \lang1049\f1 
\par \lang1033\f0 Clicking on a row takes one to the line of source code with the definition or usage.
\par \lang1049\f1 
\par }
440
Scribble440
Printing
printing;



Writing



FALSE
9
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Printing\cf0\b0\f1\fs20 
\par 
\par \f0 The IDE provides a range of standard printing features, including multiple copy print and collating.  The contents of any open \cf2\strike Editor Window\cf3\strike0\{linkID=50\}\cf0  may be directed to the printer by using the \b Print \b0 item on the \cf2\strike File Menu\cf3\strike0\{linkID=60\}\cf0 .
\par 
\par This documentation will not attempt to cover the features of the windows printer dialog.  However, the topic \cf2\strike Printer Settings\cf3\strike0\{linkID=540\} \cf0  discusses configuration of printouts.
\par 
\par }
450
Scribble450
Toolbars
toolbars;



Writing



FALSE
24
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Toolbars\cf0\b0\f1\fs20 
\par 
\par \f0 Orange C IDE has six toolbars.  In general the items on the toolbar also appear somewhere in the main menu or in context menus.  The toolbars appear this way when the IDE is distributed:
\par 
\par \pard\ri1480\qc\cf2\{bmc tall.png\}\cf0 
\par \pard\ri1480 
\par The toolbars are customizable via the \cf3\strike Toolbar Customization\cf2\strike0\{linkID=860\}\cf0  dialog.  Additionally, if the ALT key is held down the mouse can be used to drag buttons around on a toolbar, or off the toolbar to remove them.
\par 
\par The toolbars may be dragged to be in a different order by holding the grip bar at the left.  They may also be dragged off the menu bar entirely and placed in the workarea.  A toolbar in the workarea looks like this:
\par 
\par \pard\ri1480\qc\cf2\{bmc tdebugclient.png\}\cf0 
\par \pard\ri1480 
\par For more information consult the following topics:
\par 
\par \pard\ri1440\tx660\tab\cf3\strike Edit Toolbar\cf2\strike0\{linkID=740\}
\par \tab\cf3\strike Navigation Toolbar\cf2\strike0\{linkID=770\}
\par \tab\cf3\strike Bookmark Toolbar\cf2\strike0\{linkID=780\}\cf0 
\par \pard\ri1460\tx680\tx3680\tab\cf3\strike Build Toolbar\cf2\strike0\{linkID=750\}
\par \cf0\tab\cf3\strike Debug Toolbar\cf2\strike0\{linkID=760\}\cf0 
\par \tab\cf3\strike Thread Toolbar\cf2\strike0\{linkID=790\}
\par \tab\cf3\strike Build Type Toolbar\cf2\strike0\{linkID=755\}\cf0 
\par }
460
Scribble460
Toolbar Customization Dialog
dialog;toolbar customization dialog;toolbars;



Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Toolbar Customization Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 When a toolbar is being \cf3\strike customized\cf4\strike0\{linkID=860\}\cf2 , the Toolbar Customization Dialog appears.  This is a standard windows dialog box.  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc tbcustom.png\}
\par \pard 
\par \cf2 This dialog configures placement of items in the toolbar, but does not affect where on the display the toolbar is placed.
\par \cf4 
\par \cf2 The left pane shows items that aren't in the toolbar.  The right pane shows items currently in the toolbar, in the order which they appear.  
\par 
\par To move an item into the toolbar, select the item in the left pane and its position in the right pane, and press the \b Add\b0  button.
\par 
\par To remove an item from the toolbar, select it in the right pane and press the \b Remove\b0  button.
\par 
\par Separators may be inserted anywhere in the toolbar.
\par 
\par \b Close\b0  closes the dialog box; changes are accepted.
\par 
\par \b Reset\b0  resets the toolbar to its default configuration.  
\par 
\par \b Move up\b0  moves an item upward in the box, which is to the left in the toolbar.
\par 
\par \b Move down\b0  moves an item downward in the box, which is to the right in the toolbar.
\par 
\par \pard\ri1480\cf0\b Help\b0  displays this text.
\par 
\par In addition to customizing the toolbar with this Dialog, pressing shift and clicking on an item in an active toolbar will allow you to drag the item elsewhere on the tool bar, or drag it off the toolbar
\par \pard\cf2 
\par \cf1\b\fs32 
\par }
470
IDH_CHOOSE_COLOR_DIALOG
Color Dialog
color dialog;dialog;



Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Color Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Color Dialog is accessed either from the \cf3\strike General Properties Dialog\cf4\strike0\{linkID=550\}\cf2  (to set colorization settings) or from the \cf3\strike Resource Image Window\cf4\strike0\{linkID=1005\}. \cf2  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc color.png\}\cf2 
\par \pard 
\par The dialog is shown here in its expanded state; normally when the dialog is first opened the palette at the right is not visible.  To open it use the \b Define Custom Colors\b0  button. To select a color choose from the palette on the left.  Alternatively add a color from the palette on the right to the Custom Colors box on the left, then choose the color from the custom colors box.\f1 
\par }
480
IDH_CHOOSE_FONT_DIALOG
Font Dialog
dialog;font dialog;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Font Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Font Dialog is accessed from the \cf3\strike General Properties Dialog\cf4\strike0\{linkID=550\}\cf2  (to set the font for \cf3\strike Editor Windows\cf4\strike0\{linkID=50\}\cf2 ) or for a \cf3\strike Resource\cf4\strike0\{linkID=1000\}\cf2 .  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc font.png\}\cf2 
\par \pard 
\par The \b Font\b0 , \b Style\b0  and \b Size\b0  items set font parameters.  The IDE uses bold and italicized fonts implicitly for colorization, so in general the style should be left as normal.
\par 
\par 
\par }
490
Scribble490
Basic Settings




Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Basic Settings\cf0\b0\f1\fs20 
\par 
\par \f0 The Basic Settings properties page is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of several of the most basic editor settings.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc basicsettings.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \b Editor Window Type\b0  sets the way edit windows are displayed.  It may be set to one of the following values:
\par \pard\fi-840\li840\tab Multiple Windows\tab multiple overlapping windows will be displayed.  Clicking on one's title bar will bring it to the front
\par \tab Tabbed Windows\tab only one window will be displayed and it will take up the available area.
\par 
\par \pard\b Show Line Numbers \b0 if set to yes, line numbers will be displayed at the left of each editor window.  Otherwise they will not be displayed.  The current line number is also always shown in the status bar.\b 
\par 
\par Mouse Wheel Lines Per Tick\b0  selects the number of lines to scroll up or down if the mouse wheel is moved one click.\b 
\par 
\par Editor Font \b0 selects the font to be used in the editor.  When the arrow to the right of the font name is clicked, a \cf2\strike Font Dialog\cf3\strike0\{linkID=480\}\cf0  will appear to help locate a new font.\b 
\par \b0 
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par }
500
Scribble500
Colors




Writing



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Colors\cf0\b0\f1\fs20 
\par 
\par \f0 The Color Settings properties page is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of colors used in the editor presentation.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc colorsettings.png\}
\par 
\par \pard\cf0 This page has a list of available colors, and shows a bar with each color next to its name.  Clicking on the arrow at the right of the bar brings up a \cf2\strike Color Dialog\cf3\strike0\{linkID=470\}\cf0  which may be used to edit the color.
\par 
\par Note that the auto-coloring may be turned off via the \b\strike Colorization\b0\strike0  item in the \cf2\strike Formatting Settings\cf3\strike0\{linkID=510\}\cf0  page.
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
510
Scribble510
Formatting




Writing



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Formatting\cf0\b0\f1\fs20 
\par 
\par \f0 The Formatting Settings properties page is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of editor settings related to formatting the text.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc formatting.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \b Tabs As Spaces \b0 when set to Yes, pressing the tab key will result in a number of spaces being embedded into the text, sufficient to take the cursor to the next tab stop.  When set to no, pressing the tab key will result in a single special character meaning tab being embedded in the text.  The IDE will interpret this character to mean move the cursor to the next tab stop.\b 
\par 
\par Tab Indent\b0  this is the number of single-character column positions between tab stops.\b 
\par 
\par Auto Indent \b0 when set to yes, pressing \b Enter\b0  will cause the IDE to insert enough spaces or tabs to bring the cursor to the logical position text on the line should appear at.  For most lines this means the cursor will be lined up with the position of the first character of the previous line.  However for some C language statements such as if statements there is an additional indentation to the next tab stop, to make it easier to remember to indent such lines.\b 
\par 
\par Auto Format\b0  when set to yes some formatting will be performed automatically.  This includes moving preprocessor directives to the beginning of the line, and lining up close-brackets in the same column with the corresponding open-bracket.\b 
\par 
\par Colorize\b0  when set to yes the colors on the \cf2\strike Color Settings\cf3\strike0\{linkID=500\}\cf0  page will be applied to the text in \cf2\strike Edit Windows\cf3\strike0\{linkID=50\}\cf0 , otherwise all text will be black.\b 
\par \b0 
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
520
Scribble520
Backup Settings




Writing



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Backup Settings\cf0\b0\f1\fs20 
\par 
\par \f0 The Backup Settings is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of auto-backup settings.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc backupsettings.png\}
\par 
\par \pard\cf0 The backup settings, when set to yes, cause the corresponding types of files to be backed up just prior to saving a new version to disk.  To create a name for the backup file, the original file will have the string '.bak' appended to it.
\par 
\par For example, to back up a file \b MySourceFile.c\b0 , the IDE will create a new file \b MySourceFile.c.bak.\b0 
\par 
\par The following properties are available on this page:
\par 
\par \b Backup Files \b0 if set to yes, source code files will be backed up before being changed\b 
\par 
\par Backup Projects\b0  if set to yes, projects and workareas will be backed up before being changed
\par \b 
\par \b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
530
Scribble530
Hints And Code Completion




Writing



FALSE
42
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Hints And Code Completion\cf0\b0\f1\fs20 
\par 
\par \f0 The Hints And \cf2\strike Code Completion\cf3\strike0\{linkID=535\}\cf0  Settings is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .   It allows modification of editor settings related to hints and code completion.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc hints.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \b Editor Hints\b0  when set to yes, editor hints will be shown while not debugging.  These involve showing the declaration of types and variables.\b 
\par 
\par Debugger Hints\b0  when set to yes, debugger hints will be shown while debugging.  These involve showing the value of variables.  At this time only simple types are displayed, structures and arrays are not displayed in the hint.\b 
\par 
\par Code Completion Level\b0  sets the amount of code completion help that is desirable.  Code Completion windows give help for accessing keywords, functions, structures, and variables but the constant popups can be an annoyance.  There are three code completion levels:
\par 
\par \pard\fi-1300\li2140 Level 0\tab Code completion is off
\par Level 1\tab Accessing structure members will result in a small popup that allows selection of a member; typing in something that looks like a function call will result in a hint
\par Level 2\tab The level 1 functionality, plus a small popup that allows selection of variables and keywords
\par 
\par \pard Note that code completion only works for entities that have been defined; for example hints for strcmp may not be available if the appropriate header file which prototypes strcmp is not included as a #include in the source code.
\par 
\par \b Code Completion Threshold(Variables)\b0  sets the minimum number of characters that have to be typed as part of a variable name before the level 2 popup will appear.
\par \pard\tx840 
\par \b Code Completion Threshold(Keywords)\b0  sets the minimum number of characters that have to be typed as part of a variable name before the level 2 popup will appear.
\par 
\par \pard\b Browse Information\b0  when set to yes, compiling the project will generate information that can aid in locating the definition of a function.  At this point, the \cf2\strike Browse Menu\cf3\strike0\{linkID=170\}\cf0  items will allow locating the definition of a variable or function name.\b 
\par 
\par Match Parenthesis\b0  when set to yes, positioning the cursor next to a bracket or parenthesis will locate a matching bracket or parenthesis and highlight both.  For example positioning the cursor on an open parenthesis will locate the matching close parenthesis and highlight both.\b 
\par 
\par Column Mark\b0  allows setting of a column position, at which a vertical line will be drawn.  The vertical line gives a visual aid as to how long the line is.  For example it could be used to try to limit how long each line is (by placing line breaks in an appropriate position).\b 
\par \b0 
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
535
Scribble535
Code Completion




Writing



FALSE
11
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Code Completion\cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 Code Completion is a feature of the IDE that allows it to dynamically show a function prototype, or a list of structure members, while code is being typed in the editor.   In the C language this feature is relatively straightforward.  However in C++ there are a number of language features that are explicitly supported when typing a name for completion.   Some of these are as follows:
\par 
\par Namespace specifiers may be used to select a namespace.
\par Using statements will usually be honored.
\par Multiple functions may be listed if there are several overload matches
\par If the function currently being edited is a member function, its parent class will be used to filter the options.\lang1049\f1 
\par }
540
Scribble540
Printer Settings




Writing



FALSE
47
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Printer Settings\cf0\b0\f1\fs20 
\par 
\par \f0 The Printer Settings properties page is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of printer-related settings.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc printersettings.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \b Left Margin\b0  is the size of the left margin, in millimeters.\b 
\par 
\par Right Margin\b0  is the size of the right margin, in millimeters.\b 
\par 
\par Top Margin\b0  is the size of the top margin, in millimeters.\b 
\par 
\par Bottom Margin\b0  is the size of the bottom margin, in millimeters.\b 
\par 
\par 
\par Header \b0 and \b Footer \b0 give settings for formatting that should be performed on optional header and footer lines.  It isn't necessary to remember the shorthand for the options, as pressing the arrow to the right of the setting gives a menu with the options on it.  The menu appears as follows:
\par 
\par \pard\qc\cf3\{bmc mprintoptions.png\}\cf0\b 
\par \pard 
\par \b0 This menu has the following options, which are displayed on the settings line as shown in parenthesis:
\par 
\par \pard\tx1440\b\tab Time Formats\b0  include 12 hour\b (%T)\b0  and 24 hour\b (%t)\b0  time formats for the current time
\par 
\par \b\tab Date Formats\b0  include the US date\b (%D) \b0 and European date\b (%d)\b0  formats for the current date
\par 
\par \b\tab Page Number\b0  of the page currently being printed\b (%P)
\par \b0 
\par \b\tab Number of Pages\b0  in source file being printed\b (%#)\b0 
\par 
\par \b\tab File Name\b0  of source file being printed\b (%F)\b0 
\par 
\par \b\tab Left(%L)\b0 , \b Right(%R)\b0 , and \b Center(%C)\b0  allow positioning of the text in various places on the line
\par \pard 
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
550
IDH_GENERAL_PROPERTIES_DIALOG
General Properties Dialog
dialog;general properties dialog;



Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\tx720\cf1\b\fs32 General Properties Dialog\cf0\b0\f1\fs20 
\par 
\par \pard\ri1480\f0 The General Properties Dialog is accessible by selecting \b Properties \b0 on the \cf2\strike Tool Menu\cf3\strike0\{linkID=120\}.\cf0   It is used for general IDE configuration which is unrelated to projects, such as color and font configuration.
\par 
\par On the left hand side of the dialog is a tree view that lists the available pages; on the right hand side properties for a page will be shown.  Following is an example of how one of the general properties dialog pages appears.
\par 
\par \pard\ri1480\qc\cf3\{bmc hints.png\}\cf0 
\par \pard\ri1480 
\par \pard\tx720 This dialog has the following panes:
\par 
\par \pard\tx720\tx1400\b Editor Settings\b0 
\par \pard\tx1380\tx4220\tab\cf2\strike Basic Settings\cf3\strike0\{linkID=490\}\tab\cf0 Simple settings such as the font
\par \tab\cf2\strike Colors\cf3\strike0\{linkID=500\}\cf0\tab Colors to be used when highlighting source code elements
\par \tab\cf2\strike Formatting\cf3\strike0\{linkID=510\}\cf0\tab Settings related to text spacing and formatting
\par \tab\cf2\strike Backup Settings\cf3\strike0\{linkID=520\}\cf0\tab File backup parameters\cf3 
\par \pard\tx720\tx1380\tx4240 
\par \pard\tx720\tx1400\cf0\b Other Settings\b0 
\par \pard\tx1380\tx4220\tab\cf2\strike Hints and Code Completion\cf3\strike0\{linkID=530\}\tab\cf0 Settings related to automatic help given by the editor
\par \tab\cf2\strike Printer Settings\cf3\strike0\{linkID=540\}\cf0\tab Printer Settings
\par \tab\cf2\strike Win32 Help configuration\cf3\strike0\{linkID=555\}\cf0\tab Help file settings for the WIN32 help
\par 
\par \pard\tx720 Properties specific to building programs are on the \cf2\strike Project Properties\cf3\strike0\{linkID=580\}\cf0  dialog\f1 
\par }
555
Scribble555
WIN32 Help Configuration
help;help menu;



Writing



FALSE
23
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 WIN32 Help Configuration\cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 The WIN32 Help Configuration properties page is inside the \cf2\strike General Properties Dialog\cf3\strike0\{linkID=550\}\cf0 .  It allows modification of settings related to setting up WIN32 help.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc helpsettings.png\}
\par \pard\cf0\lang1049\f1 
\par \lang1033\f0 When WIN32 help is configured the key sequence \b CTL-F1 \b0 will use the WIN32 help to locate the word under the cursor.
\par 
\par The following properties are available on this page:
\par 
\par \b Win32 Help Mode\b0  is the type of win32 help that is configured.   It is one of the following:
\par \pard\tx1420\tx4240 
\par \tab\b Online Help\b0\tab This uses an internet connection to access MSDN in online format
\par \tab\b Windows 8 Offline help\b0\tab This allows using the MSDN help on a local computer
\par \tab\tab A configuration step downloads the help
\par \tab\b WIN32.CHM\b0\tab This uses the old Win32 help, which has been converted to CHM format.
\par 
\par \b CHM File Path\b0  is the complete path where the WIN32.CHM help file resides.  It is only used if the mode is WIN32.CHM
\par 
\par \lang1049\f1 
\par }
560
Scribble560
General Project Configuration Properties




Writing



FALSE
46
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 General Project Configuration Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The General Project Configuration Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to the type of project.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc general.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \b Output Path\b0  selects the place to store the intermediate compilation files, and the output executable or library file.  It is normally set to \b $(PROJECTDIR)\b0 .\b 
\par 
\par Output File\b0  selects the name of the output executable or library.  It is normally set to \b $(OUTPUTDIR)\\$(OUTPUTNAME)$(OUTPUTEXT).\b0  where \b $(OUTPUTDIR)\b0  is derived from the output path, \b $(OUTPUTNAME)\b0  is from the project name, and \b $(OUTPUTEXT)\b0  is an operating-system dependent file extension such as .EXE .DLL or .LIB that depends on the Project Type setting.\b 
\par 
\par Project Type \b0 selects the project type.  The following project types are available:
\par \b 
\par \pard\tx1440\tx4240\tab CONSOLE\tab\b0 an application designed to have a text mode interface.  This is the type of application C compilers normally create.\b 
\par \tab GUI\tab\b0 an application designed to have a windowing interface.\b 
\par \tab DLL\tab\b0 a shared library that holds code that may be executed by the operating system\b 
\par \tab Static Library\tab\b0 a shared library that holds code that may be combined with other code to make an application or DLL.\b 
\par \tab DOS\b0\tab an MSDOS application.\b 
\par \tab RAW\b0\tab an application that is raw, designed for no OS in particular.  The runtime library would not work with it very well.
\par \b 
\par \pard Library Type\b0  selects the type of C Language run-time library to use for standard functions like \f2 printf\f0 , when creating an application executable.  The following library types are available:
\par 
\par \pard\tx1420\tx4240\b\tab None\tab\b0 don't use a library.  Project types other than Static Library and Raw that don't use a library will not run under an OS.\b 
\par \tab Static Library\tab\b0 take the code from the library and put it directly in the application\b 
\par \tab LSCRTL.DLL\b0\tab use the LSCRTL.DLL that comes with this package as the library.\b 
\par \tab CRTDLL.DLL\b0\tab use the OS file CRTDLL.DLL as the library\b 
\par \tab MSVCRT.DLL\b0\tab use the OS file MSVCRT.DLL as the library\b 
\par 
\par \tab\b0 Note that in addition to the C language library, the IDE may add a static library that creates links to windowing functions.\b 
\par 
\par \pard Unicode \b0 when set to Yes, the UNICODE version of the windowing functions will be selected.  Otherwise, the ASCII version will be selected.  This does not affect functions like \f2 printf\f0  which are in the C language runtime library.\b 
\par \b0 
\par \b Show detailed build info\b0 , when set to YES, displays the commands used during the build process, as the project is being built.
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
570
Scribble570
Debug Project Config Properties




Writing



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Debug Project Config Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Debug Project Configuration Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to debugging the project. It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc debug.png\}
\par \pard 
\par \cf0 Note there is also a setting on the \cf2\strike Hints And Code Completion\cf3\strike0\{linkID=530\}\cf0  properties page which enables or disables debugger hints.  Debugger hints show the value of variables when the cursor is placed over them.\cf3 
\par \pard\qc 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \pard\tx1420\tx4240\b Create Debug Info\b0 , when set to yes creates a debug info file when the project is built\b 
\par 
\par Executable\b0  is the path to an executable to run to debug the program.  Normally it is \b $(OUTPUTFILE)\b0  which means the program generated by the project.  This is the same file specified by the \b Output File\b0  setting on the \cf2\strike General Project Configuration Page\cf3\strike0\{linkID=560\}\cf0 .  It may be necessary to change it to something else, for example when attempting to debug a DLL.\b 
\par 
\par Working Directory\b0  the path that the executed program will retrieve when it uses functions like \f2 getpwd\f0  or \f2 GetCurrentDirectory.\b 
\par \f0 
\par Command Arguments\b0  any arguments that should be passed to the executed program on the command line\b 
\par 
\par Show Return Code\b0  when set to yes, a box will appear after the program exits, showing the value that was passed to the operating system when it exited.\b 
\par 
\par Break At DLL Entry Points \b0 when set to yes, the debugger will detect the entry points of DLLs that have debug information, and stop in their initialization function such as \f2 DLLMain\f0 .\b 
\par 
\par Stop On First Chance Exception \b0 when set to no, the debugger will give the program a chance to handle any exception generated by the operating system, before declaring an error and stopping.  When set to yes, the debugger will stop immediately when an exception is raised, without giving the program a chance to handle it.
\par \b 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par }
580
IDH_PROJECT_PROPERTIES_DIALOG
Project Properties Dialog
dialog;target properties dialog;



Writing



FALSE
41
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Project Properties Dialog\cf0\b0\f1\fs20 
\par 
\par \pard\ri1480\f0 The Project Properties Dialog is accessible by selecting \b Properties \b0 on the \cf2\strike Workarea Context Menu\cf3\strike0\{linkID=210\},\cf0   the \cf2\strike Project Context Menu\cf3\strike0\{linkID=230\}\cf0 , or the \cf2\strike File Context Menu\cf3\strike0\{linkID=240\}\cf0 .  It can also be reached by selecting \b Properties\b0  from the \cf2\strike Project Menu\cf3\strike0\{linkID=110\}\cf0 ; this specifically opens a Properties window associated with the active project.  It is used for general IDE configuration related to building projects.
\par 
\par The Project Properties are tiered in several levels.  The same settings appear internally on each level.  
\par 
\par The top level is for the default values for properties.  These are the settings which are used by default.  
\par 
\par The default values may be overridden by selecting the properties menu item on the Workarea Context menu.  Setting values at this level sets the properties for all projects and files.
\par 
\par The Workarea properties may be overridden by selecting the properties menu item on the context menu for any project.  Setting values at this level sets the properties for the project and all files it contains, but does not affect other projects.
\par 
\par The project properties may be further overridden by selecting the properties menu item on the context menu for any file.  Setting values at this level sets the properties for the file but does not affect any other files or projects.
\par 
\par The project properties work internally by utilizing built-in macros, some of which are exposed on the property pages.  An example of this is \b PROJECTDIR\b0  which expands to the location where the project file is located.  The properties system also imports the user's environment variables, so it is possible to define entities outside the IDE and use them as parameters.
\par 
\par At the top of the project properties dialog are selection boxes which allow selection of which \cf2\strike Profile\cf3\strike0\{linkID=585\}\cf0  the settings apply to.  These selection boxes default to the currently selected profile.   There are two selection boxes, the 'profile' selection box which names a profile, and the 'build type' selection box which allows a choice between release and debug modes.  Note that these selection boxes do not change the currently selected profile, only the profile that new settings will apply to.
\par 
\par On the left hand side of the dialog is a tree view that lists the available pages; on the right hand side properties for a page will be shown.  Following is an example of how one of the project properties dialog pages appears.
\par 
\par \pard\ri1480\qc\cf3\{bmc pictures\\toolproperties\\linker.png\}\cf0 
\par \pard\ri1480 
\par The Project Properties Dialog has the following panes:
\par 
\par \pard\ri1460\tx1420\tx5700\b General Properties\b0 :
\par \tab\cf2\strike General Project Configuration\cf3\strike0\{linkID=560\}\cf0\tab Configuration about the project in general
\par \tab\cf2\strike Debug Project Configuration\cf3\strike0\{linkID=570\}\cf0\tab Configuration about how to debug the project
\par 
\par \b Tools Properties\b0 :
\par \tab\cf2\strike Compiler Properties\cf3\strike0\{linkID=590\}\cf0\tab Configuration for the C compiler
\par \tab\cf2\strike Assembler Properties\cf3\strike0\{linkID=600\}\cf0\tab Configuration for the Assembler
\par \tab\cf2\strike Resource Compiler Properties\cf3\strike0\{linkID=610\}\cf0\tab Configuration for the Resource Compiler
\par \tab\cf2\strike Linker Properties\cf3\strike0\{linkID=620\}\cf0\tab Configuration for the Executable File Linker
\par \tab\cf2\strike Librarian Properties\cf3\strike0\{linkID=630\}\cf0\tab Configuration for the Static Library Librarian
\par \tab\cf2\strike Custom Build Properties\cf3\strike0\{linkID=640\}\cf0\tab Configuration for things that happen after the build of a project
\par 
\par 
\par Global setup such as that for the editor is found in the \cf2\strike General Properties\cf3\strike0\{linkID=550\}\cf0  settings.
\par }
582
IDH_PROJECT_DEPENDENCIES_DIALOG
Project Dependencies Dialog
dependencies;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Project Dependencies Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 Generally the IDE keeps track of header file dependencies automatically, by parsing source files when something is added or changed and using the embedded #include statements to keep track of the header files the file depends on.
\par 
\par However, when there are multiple projects in the WorkArea some of them may depend on each other.   For example, an executable may depend on one more more static libraries.
\par 
\par The IDE cannot determine these types of dependencies without help.   The Project Dependencies dialog allows you to set these dependencies.   It appears as follows:
\par \cf2 
\par \pard\qc\cf3\{bmc projdepends.png\}\cf0 
\par \pard 
\par In this dialog, select the project whose dependencies are to be set in the upper combo box.   Then a list of the other projects in the WorkArea
\par will appear and it is possible to select which other projects this project depends on.
\par 
\par Note that sometimes, a project in this list will be grayed out and will not be selectable.   This happens because these screens create a dependency tree, and clicking on that item would cause a loop in the tree as that item already depends on the item being worked on.  The IDE does not allow this situation to occur.
\par 
\par The dependency tree is eventually flattened.   This is to make a list of projects in the order they should be built in order to make sure that dependencies of a project are always built before the project.   To view the projects in the order they will be built select 'View Build Order'.  The dependencies dialog will then look as follows:
\par \cf2 
\par \pard\qc\cf3\{bmc projbuildorder.png\}\cf0 
\par \pard 
\par If a project depends on a static library, the project will be linked against that static library after the library is built.
\par 
\par When done setting dependencies, select 'OK'.
\par 
\par }
585
Scribble585
Profiles




Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Profiles\cf0\b0\f1\fs20 
\par 
\par \f0 Profiles are a convenient way to allow creation of different groups of settings, and associate them with a profile name.  By \cf2\strike selecting a profile\cf3\strike0\{linkID=589\}\cf0 , an entire group of settings may be selected, which will then be used in subsequent build operations.  A \cf2\strike New Profile\cf3\strike0\{linkID=587\}\cf0  may also be added.
\par 
\par For example, it might be convenient to create a testing profile that selects specific preprocessor definitions to put a project in a test mode.
\par 
\par There are two build types associated with each profile; the release type and the debug type.  The difference is that the release type will use compiler optimizations, and will not have debug information.  The debug type will not use compiler optimizations, but will have debug information suitable for debugging the program.
\par 
\par In the default case the selected profile name and build type are used as part of the directory name which indicates where to put the output files, however, this behavior can be altered through build settings.
\par 
\par \f1 
\par }
587
Scribble587
New Profile Dialog




Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 New Profile Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The New Profile dialog is accessible from the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows addition of a new \cf2\strike profile\cf3\strike0\{linkID=585\}\cf0  name that will allow creating an independent set of settings for a build process.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc newprofile.png\}
\par \pard 
\par \cf0 Typing a new name and selecting 'ok' will result in the profile being added to the list of possible profiles.  It will have the default configuration.\f1 
\par }
589
IDH_SELECT_PROFILE_DIALOG
Select Profile Dialog




Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Select Profile Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The Select Profile dialog is accessible by selecting \b Select Profile\b0  from the  \cf2\strike Project Menu\cf3\strike0\{linkID=110\}\cf0 .  It allows selection of which \cf2\strike profile\cf3\strike0\{linkID=585\}\cf0  will be used for subsequent builds.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc selectprofile.png\}
\par \pard\cf0 
\par The \b Profile Name\b0  selection box selects the profile; the \b Build Type \b0 selection box selects whether to build in debug or release mode.  Selecting a profile and pressing ok will cause the selected profile to be used in subsequent build operations.\f1 
\par }
590
Scribble590
Compiler Properties




Writing



FALSE
39
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Compiler Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Compiler Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows allows modification of project settings related to compiling individual C language source files with the C language compiler.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc compiler.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \pard\tx1420\tx4240\b Additional Preprocessor Directives\b0  is a space-delimited list of macro declarations.  This is a way to duplicate #define behavior from the command line, which allows some definitions to be kept as part of the build process instead of forcing them to reside in header files.  The macro definition has the form \f2 macroname=value\f0  or simply \f2 macroname \f0 if it is to be defined without a value.  If it is required to put spaces within the value, enclose the entire thing in quotes, for example: "\f2 macroname=my value".\b\f0 
\par 
\par Additional Include Paths\b0  is a semi-colon delimited list of paths to search for include files.  If relative paths are specified, they will be relative to the directory in which the project file is stored.\b 
\par 
\par Additional Dependencies\b0  specifies a list of additional dependencies for the file.  Normally, the IDE figures out the dependencies and it isn't necessary to use this.\b 
\par 
\par \pard Output File\b0  is the name of the output file.  It is normally set to \b $(OUTPUTDIR)\\$(OUTPUTNAME)$(OUTPUTEXT).\b0  where \b $(OUTPUTDIR)\b0  is derived from the output path, \b $(OUTPUTNAME)\b0  is the stem of the source file name, and \b $(OUTPUTEXT)\b0  is the literal '.obj'.\b 
\par \pard\tx1420\tx4240 
\par C99 \b0 is set to yes if parsing for C99 language constructs should be allowed.  Otherwise the compiler will be limited to C89 language constructs.\b 
\par 
\par Ansi\b0  is set to yes if the compiler should generate an error if one of several commonly used extensions to the ANSI standard is used.  For example, in C89 it is common for compilers to allow a comma at the end of a list of enumerations, however, the C89 ansi standard forbids it and turning on ANSI mode will disable the extension.\b 
\par 
\par Show Warnings\b0  is set to yes if the IDE should show both Warnings and Errors in the \cf2\strike Information Window\cf3\strike0\{linkID=280\}\cf0  during the build process.  Otherwise only Errors will be shown.\b 
\par 
\par Align Stack \b0 is set to yes if the compiler should generate extra code to keep the processor stack aligned to the nearest 16-byte increment when entering a new function.  This option can speed up floating point code that uses 'double' values liberally.\b 
\par 
\par Additional Switches\b0  selects additional compiler switches which aren't directly supported by the IDE.  Any switch that can be specified on the command line may be placed here.  For example, putting \b +i \b0 here will cause the compiler to generated a preprocessed output file.\b 
\par 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par 
\par \f1 
\par 
\par }
600
Scribble600
Assembler Properties




Writing



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Assembler Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Assembler Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to compiling individual Assembly language source files with the assembler.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc assembler.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par \pard\tx1420\tx4240\b 
\par Additional Preprocessor Directives\b0  is a space-delimited list of macro declarations.  This is a way to duplicate %define behavior from the command line, which allows some definitions to be kept as part of the build process instead of forcing them to reside in header files.  The macro definition has the form \f2 macroname=value\f0  or simply \f2 macroname \f0 if it is to be defined without a value.  If it is required to put spaces within the value, enclose the entire thing in quotes, for example: "\f2 macroname=my value".\b\f0 
\par 
\par Additional Include Paths\b0  is a semi-colon delimited list of paths to search for include files.  If relative paths are specified, they will be relative to the directory in which the project file is stored.\b 
\par 
\par Additional Dependencies\b0  specifies a list of additional dependencies for the file.  Normally, the IDE figures out the dependencies and it isn't necessary to use this.\b 
\par 
\par Additional Switches\b0  selects additional compiler switches which aren't directly supported by the IDE.  Any switch that can be specified on the command line may be placed here.  For example, putting \b -UMyMacro\b0  here globally undefines the macro \b MyMacro.
\par \pard\b0 
\par \b Output File\b0  is the name of the output file.  It is normally set to \b $(OUTPUTDIR)\\$(OUTPUTNAME)$(OUTPUTEXT).\b0  where \b $(OUTPUTDIR)\b0  is derived from the output path, \b $(OUTPUTNAME)\b0  is the stem of the source file name, and \b $(OUTPUTEXT)\b0  is the literal '.obj'.\b 
\par \pard\tx1420\tx4240 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par \f1 
\par }
610
Scribble610
Resource Compiler Properties




Writing



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Resource Compiler Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Resource Compiler Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to compiling individual resource files with the resource compiler.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc rc.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \pard\tx1420\tx4240\b Additional Preprocessor Directives\b0  is a space-delimited list of macro declarations.  This is a way to duplicate #define behavior from the command line, which allows some definitions to be kept as part of the build process instead of forcing them to reside in header files.  The macro definition has the form \f2 macroname=value\f0  or simply \f2 macroname \f0 if it is to be defined without a value.  If it is required to put spaces within the value, enclose the entire thing in quotes, for example: "\f2 macroname=my value".\b\f0 
\par 
\par Additional Include Paths\b0  is a semi-colon delimited list of paths to search for include files.  If relative paths are specified, they will be relative to the directory in which the project file is stored.\b 
\par 
\par Additional Dependencies\b0  specifies a list of additional dependencies for the file.  Normally, the IDE figures out the dependencies and it isn't necessary to use this.\b 
\par 
\par Additional Switches\b0  selects additional compiler switches which aren't directly supported by the IDE.  Any switch that can be specified on the command line may be placed here.\b 
\par \pard\b0 
\par \b Output File\b0  is the name of the output file.  It is normally set to \b $(OUTPUTDIR)\\$(OUTPUTNAME)$(OUTPUTEXT).\b0  where \b $(OUTPUTDIR)\b0  is derived from the output path, \b $(OUTPUTNAME)\b0  is the stem of the source file name, and \b $(OUTPUTEXT)\b0  is the literal '.res.\b 
\par \pard\tx1420\tx4240 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par \f1 
\par 
\par }
620
Scribble620
Linker Properties




Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Linker Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Linker Properties page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to creating executable files.  It is only available when the project is some executable type like CONSOLE or GUI.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc linker.png\}
\par 
\par \pard\cf0 The following properties are available on this page:\b 
\par \b0 
\par \pard\tx1420\tx4240\b Additional Libraries\b0  is a semi-colon delimited list of libraries to search to resolve code references.\b 
\par 
\par Additional Switches\b0  selects additional linker switches which aren't directly supported by the IDE.  Any switch that can be specified on the command line may be placed here.  For example \b FileAlign:1024 \b0 sets the PE file alignment parameter to 1024.\b 
\par \pard\b0 
\par 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par \f1 
\par 
\par 
\par }
630
Scribble630
Librarian Properties




Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Librarian Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Librarian Properies page is inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows modification of project settings related to building static libraries.  It is only available when the project type is Static Library.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc librarian.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \pard\tx1420\tx4240\b Page Size\b0  gives an internal alignment value.  It is a power of two greater than or equal to 16.  Lower values result in smaller libraries, but the smaller size is also a limitation so sometimes it is desirable to use larger values.  The default is 512.
\par \b 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par \f1 
\par 
\par 
\par }
640
Scribble640
Custom Build Properties




Writing



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Custom Build Properties\cf0\b0\f1\fs20 
\par 
\par \f0 The Custom Build properties page are inside the \cf2\strike Project Properties Dialog\cf3\strike0\{linkID=580\}\cf0 .  It allows defining something that can happen after a project is built.  By default it does nothing.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc custom.png\}
\par 
\par \pard\cf0 The following properties are available on this page:
\par 
\par \pard\tx1420\tx4240\b Display \b0 is a text string to display in the \cf2\strike Information Window\cf3\strike0\{linkID=280\}\cf0  when starting this build step.\b 
\par 
\par Commands \b0 is a list of commands.  Note: this version of the IDE only allows one command to be entered.\b 
\par 
\par Output Files \b0 is a list of files that will be generated by this build step.  Note: this version of the IDE only allows one output file to be entered.\b 
\par 
\par Additional Dependencies\b0  specifies a list of additional dependencies for this build step.  For this type of build step, the IDE cannot figure out any dependencies for the build step, and anything that needs to be pre-built would have to be specified here.\b 
\par 
\par \pard\b0 
\par \b Apply\b0  saves changes but leaves the dialog open
\par 
\par \b Close\b0  closes the dialog without saving changes
\par 
\par \b Accept\b0  saves changes then closes the dialog
\par 
\par \f1 
\par 
\par }
650
IDH_WATCH_WINDOW
Watch Window
debugging windows;locals window;special windows;watch window;windows;



Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Watch Window\cf0\b0\f1\fs20 
\par 
\par \f0 The watch window is accessible from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It  shows the value of variables, and allows them to be modified.  It appears similar to what follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc wwatch.png\}\cf0 
\par \pard\ri1480 
\par \pard\cf4 There are four watch windows that may be used independently of each other.
\par \b 
\par \pard\ri1480\cf0\b0 When a value is displayed, clicking on the right-hand side of the value will open a small text window.  Typing a new value then pressing ENTER will cause that value to be written to the executable's memory.
\par 
\par Right-Clicking in the watch window opens the \cf2\strike Watch Context Menu\cf3\strike0\{linkID=660\}\cf0 .  This allows adding or removing items to watch.
\par 
\par Right-Clicking in an edit window will display the \cf2\strike Editor Context Menu\cf3\strike0\{linkID=150\}\cf0 .  The \b Add To Watch\b0  menu item will display the word under the cursor in the current tab of the window, along with its value.
\par 
\par The expression for the watch may be complex, utilizing namespace selectors and structure access operators.
\par 
\par Values that have changed since the last time the program stopped at a breakpoint will be in red; other values will be in black.
\par 
\par The window is updated automatically each time the program stops at a break point.
\par \pard\fi-2800\li2800\ri1460\tx720\tx2820\f1 
\par }
655
Scribble655
Locals Window
locals window;watch window;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Locals Window\cf0\b0\f1\fs20 
\par \pard\ri1480 
\par \f0 The watch window is accessible from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It  shows the value of variables which are currently in scope, and allows them to be modified.  It appears similar to what follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc wlocals.png\}\cf0 
\par \pard\ri1480 
\par \pard\cf4 This window is another kind of \cf2\strike Watch Window\cf3\strike0\{linkID=650\}, \cf0 the difference is that the locals window automatically selects the local (auto) variables which are currently in scope so that they don't have to be individually added to a watch window.   In a C++ member function, it will also show the this pointer.\cf4 
\par \b 
\par \pard\ri1480\cf0\b0 When a value is displayed, clicking on the right-hand side of the value will open a small text window.  Typing a new value then pressing ENTER will cause that value to be written to the executable's memory.
\par 
\par The window is updated automatically each time the program stops at a break point.
\par \pard\fi-2800\li2800\ri1460\tx720\tx2820\f1 
\par \pard\lang1049 
\par }
660
Scribble660
Watch Context Menu
context;menu;watch context menu;



Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Watch Context Menu\cf2\b0\f1\fs20 
\par 
\par \f0 The Watch Context Menu is accessed by right-clicking in the \cf3\strike Watch Window\cf4\strike0\{linkID=650\}\cf2 .  It has menu items used for working with the Watch Window.  \cf0 It appears similar to what follows:\cf2 
\par 
\par \pard\qc\cf4\{bmc cmwatch.png\}\cf2 
\par \pard 
\par \b Add To Watch\b0  opens the \cf3\strike Add To Watch Dialog\cf4\strike0\{linkID=370\}\cf2  to allow a variable name to be specified.  The variable will be displayed along with its value in the current tab of the Watch Window.  If the watch window is presently closed it will be opened.
\par 
\par \b Remove From Watch\b0  removes the selected item from the Watch Window.
\par 
\par \b Clear Watch Window\b0  removes all items from the Watch Window\f1 
\par }
670
IDH_ASSEMBLY_WINDOW
Disassembly Window
assembly window;debugging windows;special windows;windows;



Writing



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Disassembly Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Assembly Window is accessed from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It shows assembly language instructions for the program.  It appears similar to what follows:
\par 
\par \pard\ri1460\qc\cf3\{bmc wasm.png\}\cf0 
\par \pard\ri1460 
\par Generally the window will start at the position the break point is at.  However if focus is set to the window, then the 'goto' toolbar item (or CTRL-G) will bring up a dialog allowing the address to be changed.  A number can be entered here, or the name of a variable.  It is also possible to type an \cf2\strike expression\cf3\strike0\{linkID=800\}\cf0  into this box and have it evaluated.
\par 
\par In the window, disassembled instructions will be shown in a light gray and commentary showing the source code will be in a darker gray.  
\par 
\par While the window has focus, the up and down arrows and page up and page down keys will allow navigation through the disassembly.  As a convenience, pressing the HOME key will show the executable code address where the program counter is currently stopped.
\par 
\par While the disassembly windows has focus, debugger single-step commands will be for individual lines of disassembled code, instead of for source code file lines.
\par 
\par Clicking on a source code line will bring up an \cf2\strike editor window\cf3\strike0\{linkID=50\}\cf0  to allow editing the source code.   Clicking on a disassembled instruction will result in a \cf2\strike breakpoint\cf3\strike0\{linkID=410\}\cf0  being set or cleared.
\par 
\par }
680
IDH_MEMORY_WINDOW
Memory Window
debugging windows;memory window;special windows;windows;



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Memory Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Memory Window is accessed from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It appears similar to what follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc wmem.png\}\cf0 
\par \pard\ri1480 
\par There are four memory windows which may be used independently of each other.
\par 
\par The address is shown on the left, followed by a sequence of bytes, followed by a sequence of ASCII values for the byte sequence.  By default 16 bytes will be shown per line, however if the window is resized it will automatically adjust the number of bytes per line to fit.
\par 
\par At the top of the window is a small text box which is used to tell what memory should be viewed.  An address can be entered here, or the name of a variable.  It is also possible to type an \cf2\strike expression\cf3\strike0\{linkID=800\}\cf0  into this box and have it evaluated.   A toolbar button is used to choose whether the address should remain static or whether it should be re-evaluated every time the program stops at a breakpoint.
\par 
\par When the memory portion of the window has keyboard focus, a black cursor will appear over one of the values.  The cursor may be moved around to select a value by using the arrow and page keys.  Typing in a Hexadecimal digit will turn the cursor to blue; a second digit can be typed at this time.  While the cursor is blue, the ESCAPE key will void the edit function and return the cursor to black, or the ENTER key will accept the new value and attempt to write it to the program's memory space.  If successful the display will update with a black cursor and show the new value; if the new value cannot be written then a dialog will appear to inform you of the problem.
\par 
\par Values which have changed since stopping at the last breakpoint will be in red; other values will be in black.
\par 
\par The window is automatically updated whenever the program stops at a break point.  
\par }
690
Scribble690
Memory Context Menu




Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Memory Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The Memory Context Window is accessed by right-clicking in the \cf2\strike Memory Window\cf3\strike0\{linkID=680\}\cf0 .  It allows selection of the size of the data displayed and the number of bytes of data to show per line.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc cmmemory.png\}\cf0 
\par \pard 
\par The following data sizes are allowed:
\par 
\par BYTE - selects a byte per unit (two hexadecimal characters)
\par 
\par WORD - selects a word per unit (two bytes or four hexadecimal characters)
\par 
\par DWORD - selects a dword per unit (four bytes or eight hexadecimal characters)
\par \f1 
\par \f0 The auto selection for bytes per line will adjust the number of bytes shown based on the window width. \f1 
\par }
700
IDH_STACK_WINDOW
Call Stack Window
debugging windows;special windows;stack window;windows;



Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Call Stack Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Call Stack Window is accessible from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  shows a back trace of all functions that were called on the way to hitting the current break point.  This information is retrieved from a processor location known as the stack.  The window appears similar to what follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc wstack.png\}\cf0 
\par \pard\ri1480 
\par The window shows the function address and the name when debug information is available.  Sometimes debug information won't be available, such as for code that is stopped in a C language library function, or in the operating system function.  
\par 
\par The back trace will be shown for the thread that is selected in the \cf2\strike Thread Window\cf3\strike0\{linkID=710\}.\cf0   Changing which function is selected by clicking on it will be reflected in the \cf2\strike Thread Toolbar\cf3\strike0\{linkID=790\}.\cf0 
\par 
\par \pard\ri1440\tx1420\tx4240 Additionally, \cf2\strike expression\cf3\strike0\{linkID=800\}\cf0  calculations depend on the scope set up by this selection.\b 
\par \pard\ri1480\b0 
\par The back trace is shown in reverse order, with the most recent function at the top.
\par 
\par 
\par }
710
IDH_THREAD_WINDOW
Thread Window
debugging windows;special windows;thread window;windows;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Thread Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Thread Window is accessible from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It shows currently active threads.  It appears similar to what follows:
\par 
\par \pard\ri1460\qc\cf3\{bmc wthread.png\}\cf0 
\par \pard\ri1460 
\par This window shows the address each thread is at, and the name of the function it is in when available.  The thread that is at a break point is marked with a yellow arrow.
\par 
\par When a new thread is selected by clicking on it, the \cf2\strike Thread Toolbar\cf3\strike0\{linkID=790\}\cf0  will be updated, and the \cf2\strike Call Stack\cf3\strike0\{linkID=700\}\cf0  and \cf2\strike Register\cf3\strike0\{linkID=720\}\cf0  windows will reflect the contents of the selected thread.
\par }
720
IDH_REGISTER_WINDOW
Register Window
debugging windows;register window;special windows;windows;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Register Window\cf0\b0\f1\fs20 
\par 
\par \f0 The Register Window is accessible from the \cf2\strike Debug->Windows Menu\cf3\strike0\{linkID=320\}\cf0 .  It shows information about the CPU registers.  It looks similar to what follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc wregister.png\}\cf0 
\par \pard\ri1480 
\par This window has three sections.  The Arithmetic section shows information about the basic register set used for memory and mathematics operations.  The Control section shows information about registers used in CPU control, such as the program counter and stack pointer.  The Floating Point section shows information about the floating point registers.
\par 
\par Register values that changed between the last two breakpoints are shown in red; values that did not change are shown in black.  Clicking on a register value allows it to be edited.
\par 
\par \pard The register values shown will be for the thread selected in the \cf2\strike Thread Window\cf3\strike0\{linkID=710\}.
\par 
\par \cf0\f1 
\par }
730
IDH_HARDWARE_BREAKPOINTS_DIALOG
Hardware Breakpoints Dialog
breakpoints;hardware breakpoints;



Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Hardware Breakpoints Dialog\cf2\b0\f1\fs20 
\par 
\par \f0 The Hardware Breakpoints Dialog is accessible by selecting \b Hardware Breakpoints\b0  from the \cf3\strike Debug Menu\cf4\strike0\{linkID=310\}\cf2 .  It is used to set special microprocessor breakpoints that can be triggered on data access.  It appears similar to what follows:
\par 
\par \pard\qc\cf4\{bmc hardwarebreak.png\}\cf2 
\par \pard\f1 
\par \pard\ri1440\tx2100\f0 Hardware breakpoints are somewhat limited.  There are only four of them, and they can only be set on values that are integers or smaller.  \cf3\strike Data Breakpoints\cf4\strike0\{linkID=890\}\cf2  can often be a better approach, however, hardware breakpoints do not slow down the program like data breakpoints do.
\par 
\par Each Hardware Breakpoint has an associated name box, which holds an \cf3\strike Expression\cf4\strike0\{linkID=800\}\cf0  that determines the address the break point starts at.\cf2 
\par 
\par The breakpoints can be configured for an access \b Size\b0  in 1, 2, or 4 byte increments and set to an \b Access Type\b0  of either Write or Access.  The Write mode causes the program to stop of data is written to the specified address.  The Access mode causes the program to stop if the data is either read or written.
\par 
\par Additionally, the break point may be disabled to prevent it from triggering.
\par 
\par 
\par }
740
IDH_EDIT_TOOLBAR
Edit Toolbar
edit toolbar;toolbars;



Writing



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Edit Toolbar\cf0\b0\f1\fs20 
\par 
\par \pard\ri1460\f0 The Edit toolbar has controls which are commonly used when working with source files.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0 .  In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par \pard\ri1480 
\par \pard\ri1480\qc\cf3\{bmc tedit.png\}\cf0 
\par \pard\ri1480\f1 
\par \f0 The Edit toolbar has the following options:
\par \pard\fi-2800\li2800\ri1460\tx700\tx2800 
\par \pard\fi-4240\li4240\ri1440\tx1400\tx4260\tab\b Save\b0\tab Save the current file to disk
\par \tab\b Save All\b0\tab Save all open files to disk
\par \tab\b Print\b0\tab Open the Printer Dialog in preparation for printing the file.
\par 
\par \tab\b Cut\b0\tab Cut the selection and put it on the clipboard
\par \tab\b Copy\b0\tab Copy the selection to the clipboard
\par \tab\b Paste\b0\tab Paste the clipboard at the current cursor position
\par \tab\b Undo\b0\tab Undo the last thing done
\par \tab\b Redo\tab\b0 Redo the last thing undone
\par 
\par \tab\b To Upper Case\b0\tab the selected text in the edit window is converted to upper case
\par \tab\b To Lower Case\b0\tab the selected text in the edit window is converted to lower case
\par 
\par \tab\b Indent\tab\b0 Move the selected text to the right by one tab space 
\par \tab\tab This may uses either spaces or tabs depending on the \cf2\strike Editor Formatting\cf3\strike0\{linkID=510\} \cf0 page.
\par \tab\b Unindent\tab\b0 Move the selected text to the left by one tab space.  Once text hits the left margin it does not move backward any further.
\par 
\par \b\tab Comment\b0\tab Add comment markers to the beginning of each line in a block of selected text.\b 
\par \tab Uncomment\b0\tab Remove comment markers which are at the beginning of lines in a block of selected text.\b 
\par }
750
IDH_BUILD_TOOLBAR
Build Toolbar
build;build toolbar;build type toolbar;make;toolbars;



Writing



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Build Toolbar\cf0\b0\f1\fs20 
\par 
\par \pard\ri1440\f0 The Build toolbar has controls for things that are commonly done while compiling and linking programs.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0 .  In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par \pard\ri1460 
\par \pard\ri1460\qc\cf3\{bmc tbuild.png\}\cf0 
\par \pard\ri1460\f1 
\par \f0 The Build toolbar has the following options:
\par \pard\fi-2800\li2800\ri1440\tx700\tx2800 
\par \pard\fi-4300\li4300\ri1440\tx1440\tx4260\tab\b Compile File\b0\tab Compile the current edit window
\par \tab\b Build All\b0\tab Build out-of-date portions of all projects
\par \tab\b Build Active\b0\tab Build out-of-date portions of the active project
\par \tab\b Rebuild All\b0\tab Rebuild every part of every project
\par \f1 
\par \f0\tab\b Stop Build\tab\b0 Stop the entire build\f1 
\par \f0 
\par 
\par }
755
IDH_BUILD_TYPE_TOOLBAR
Build Type Toolbar
build;build toolbar;build type toolbar;



Writing



FALSE
14
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Build Type Toolbar\cf0\lang1049\b0\f1\fs20 
\par 
\par \pard\ri1480\lang1033\f0 The Build Type toolbar has two windows that allows choice of the profile and the build type.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}.\cf0   In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc tbuildtype.png\}\cf0 
\par \pard\ri1480\f1 
\par \f0 The Thread toolbar has the following options:
\par \pard\fi-4200\li4200\ri1440\tx1400\tx4220\b\tab Profiles\b0\tab a combo-box which has a list of existing \cf2\strike profiles\cf3\strike0\{linkID=585\}\cf0 .   Selecting one sets the build profile.
\par \tab\b Build Type\b0\tab a combo-box which allows the choice of building in either debug or release mode.
\par 
\par 
\par }
760
IDH_DEBUG_TOOLBAR
Debug Toolbar
debug;debug toolbar;toolbars;



Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;\red0\green0\blue0;}
\viewkind4\uc1\pard\ri1460\cf1\b\fs32 Debug Toolbar\cf0\b0\f1\fs20 
\par 
\par \f0 The Debug toolbar has controls which are useful during debugging of a program.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0 .  In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par 
\par \pard\ri1460\qc\cf3\{bmc tdebug.png\}\cf0 
\par \pard\ri1460\f1 
\par \f0 The Debug toolbar has the following options:
\par \pard\fi-2800\li2800\ri1440\tx680\tx2800 
\par \pard\fi-4280\li4280\tx1420\tx4260\tx4260\tab\b Start/Continue debugging\b0\tab Start debugging, or run the program starting at the current breakpoint if it is already started.
\par 
\par \tab\tab If the debugger is being started, \cf2\strike Debugging Windows\cf3\strike0\{linkID=320\}\cf4  are opened.  The program will stop at a break point, or at the \i main()\i0  or \i WinMain() \i0 function if no break points are set.
\par \cf0 
\par \tab\b Stop Debugging\b0\tab Stop debugging and close the Debugging Windows.
\par 
\par \tab\b Run Without Debugger\b0\tab Run the program without debugging it.
\par 
\par \tab\b Toggle Breakpoint\b0\tab Toggle \cf2\strike Break Point\cf3\strike0\{linkID=410\}\cf0  at the current line in the currently selected edit window.
\par 
\par \tab\b Stop Program\b0\tab If the program is running, interrupt it and make it stop as if it hit a breakpoint.
\par 
\par \tab\b Run\b0\tab Run the program.  It will stop at the next break point encountered, or when the program exits.
\par \tab\b Run To Cursor\b0\tab Run the program.  It will stop at the next break point encountered, when the program exits, or when the program 
\par \tab\tab flow reaches the cursor position in the current active edit window.
\par 
\par \tab\b Step In\b0\tab Step into a subroutine and stop at the beginning.
\par \tab\b Step Over\b0\tab Step over a subroutine and break after the subroutine is done.
\par \tab\b Step Out\b0\tab Step out of the current function to the next position in the next enclosing function.
\par \f1 
\par \f0\tab\b Remove All Breakpoints\tab\b0 Delete all breakpoints to let the program run unobstructed.\f1 
\par }
770
IDH_NAV_TOOLBAR
Navigation Toolbar




Writing



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Navigation Toolbar\cf0\b0\f1\fs20 
\par 
\par \pard\ri1480\f0 The Navigation toolbar has controls which are commonly used to create windows with source files, or navigate through existing windows and/or source files.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0 .  In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc tnav.png\}\cf0 
\par \pard\ri1480\f1 
\par \f0 The Navigation toolbar has the following options\f1 
\par \pard\fi-4240\li4240\ri1440\tx1420\tx4240\f0 
\par \tab\b New\b0\tab Create a new text file
\par \tab\b Open\b0\tab Prompt for a file name to open, then open it in the edit window
\par 
\par \tab\b Back\tab\b0 Browse backwards through recently accessed files
\par \tab\b Forward\tab\b0 Browse forwards through recently accessed files
\par \tab\b Goto\b0\tab Open the \cf2\strike Goto Line Dialog\cf3\strike0\{linkID=340\}\cf0  to allow positioning to a specific line within the selected file
\par 
\par 
\par \tab\b Find\b0\tab Open the \cf2\strike Find/Replace Dialog\cf3\strike0\{linkID=100\}\cf0  to locate text in windows
\par \tab\b Find Next\b0\tab Locate the next occurrence of text
\par \tab\b Replace\b0\tab Open the Find/Replace Dialog to locate text in files or windows and replace it
\par \tab\b Find In Files\b0\tab Open the Find/Replace Dialog to locate text within a group of files
\par \pard\fi-2800\li2800\ri1460\tx700\tx2800 
\par \pard\ri1440\tx680\tx2780 Additionally the Navigation Toolbar has a dropdown combo box that can be used to search for text.  This form of search will search for simple text, in the current window.
\par 
\par \pard\fi-2800\li2800\ri1460\tx700\tx2800 
\par }
780
IDH_BOOKMARK_TOOLBAR
Bookmark Toolbar




Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Bookmark Toolbar\cf0\b0\f1\fs20 
\par 
\par \pard\ri1480\f0 The Bookmark toolbar has controls which are used to bookmark text in windows and navigate between bookmarks.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0 .  In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc tbrowse.png\}\cf0 
\par \pard\ri1480\f1 
\par \f0 The Bookmark toolbar has the following options:
\par \pard\fi-2800\li2800\ri1460\tx700\tx2800 
\par \pard\fi-4520\li4520\ri1440\tx1380\tx4520\tx4560\b\tab Toggle Bookmark\b0\tab enable a bookmark at the current cursor position, or disable it if it is enabled.
\par \tab\tab a bookmark indicater will show up to the left of the line if it is enabled.\b 
\par \tab Previous Bookmark\b0\tab navigate to the previous bookmark in the list\b 
\par \tab Next Bookmark\b0\tab navigate to the next bookmark in the list\b 
\par \tab Previous File With Bookmark\b0\tab navigate to the first previous bookmark in the first previous file in the list\b 
\par \tab Next File With Bookmark\b0\tab navigate to the first next bookmark in the first next file in the list\b 
\par \tab Show Bookmarks\b0\tab show the \cf2\strike Bookmarks\cf3\strike0\{linkID=390\}\cf0  Dialog to allow selecting a bookmark\b 
\par \tab Remove All Bookmarks\b0\tab remove all bookmarks from all files
\par 
\par \b 
\par \pard\ri1440\tx1420\tx2760\tx4280 
\par }
790
IDH_THREAD_TOOLBAR
Thread Toolbar




Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Thread Toolbar
\par \cf0\b0\f1\fs20 
\par 
\par \pard\ri1480\f0 The Thread toolbar has two windows that allow navigation between active threads while debugging.  It may be customized using the \cf2\strike Toolbar Customization Dialog\cf3\strike0\{linkID=860\}\cf0   In the undocked state, and when all buttons are in their normal position, it appears as follows:
\par 
\par \pard\ri1480\qc\cf3\{bmc tthread.png\}\cf0 
\par \pard\ri1480\f1 
\par \f0 The Thread toolbar has the following options:
\par \pard\fi-2800\li2800\ri1460\tx700\tx2800 
\par \pard\fi-4200\li4200\ri1440\tx1400\tx4220\b\tab Threads\b0\tab a combo-box which has a list of all running threads.  Selecting one sets the active thread to it and rese breakpoints window with the call stack of that thread.
\par \tab\b Breakpoints\b0\tab a combo-box which has a list of the call stack of the selected thread.  Selecting one opens a window with a file if it is not already open, and set the cursor to the breakpoint.
\par 
\par \pard\ri1440\tx1420\tx4240 The \cf2\strike Thread\cf3\strike0\{linkID=710\}\cf0  and \cf2\strike Call Stack\cf3\strike0\{linkID=700\}\cf0  windows will be updated with the positions selected in these combo boxes.  The contents of the Call Stack and \cf2\strike Register\cf3\strike0\{linkID=720\}\cf0  windows will be updated if selections are made on this toolbar.
\par 
\par Additionally, \cf2\strike expression\cf3\strike0\{linkID=800\}\cf0  calculations depend on the scope set up by this selection.\b 
\par }
800
Scribble800
Expressions
expressions;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Expressions\cf0\b0\f1\fs20 
\par 
\par \f0 When adding a variable to the watch window with the watch window dialog, or specifying an address in the memory window edit control, it is possible to type expressions into the dialog and have them evaluated.
\par 
\par The expression evaluator knows most of the operators of the C language.  It is also possible to type-cast expressions, to tell the watch window how to display the results of expressions.
\par 
\par In general, when a variable name is typed into the dialog, that is taken to mean the address of the variable.  However when variables are used in expressions, what happens depends on the type of the variable.  If it is a basic type such as \i int\i0  or \i short,\i0  the value is fetched and used in the expression, and the result becomes a constant.  However if it is a structured type such as \i struct\i0  or \i union\i0 , or an array value, the address is still used in the expression.  Normal C language rules apply for such things; for adding constants to structure addresses results in moving further along in an array of the structures, instead of simply offsetting the base address of the structure.
\par 
\par The expression evaluator will handle typecasting of a variable name or address.
\par 
\par Sometimes an expression requires a scope, for example a variable name may appear in several different functions, each of which has called another on the way to reaching the current stop to execution flow.  In general the IDE will use the scope specified by the \cf2\strike Thread\cf3\strike0\{linkID=710\}\cf0  and \cf2\strike Call Stack\cf3\strike0\{linkID=700\}\cf0  windows, or from the current cursor position in the case of debugger hints.   It will select from a list of global variables if it cannot locate sometihng in the local scope.
\par 
\par Each expression will be completely re-evaluated each time the program stops at a break point.\f1 
\par }
810
Scribble810
Regular Expressions
regular expressions;



Writing



FALSE
59
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\fi-1640\li1640\ri1480\tx700\tx1660\cf1\b\fs32 Regular Expressions\cf0\b0\f1\fs20 
\par \pard\ri1460\tx680\tx1640 
\par \f0 Regular expressions may be used in the query boxes of the \cf2\strike Find/Replace\cf3\strike0\{linkID=100\}\cf0  dialog.  The regular expression operators are a subset of those used by the EMACS editor.  In general characters have their normal meanings, but there are a few special characters that can be used.
\par 
\par Regular expressions work by taking a list of operators with generic meanings and trying to match parts of the source text (the edit file).  If all the operators taken together in sequence match some string in the source text, the search operation is successful.
\par 
\par \pard\fi-1640\li1640\ri1480\tx700\tx1660\tab .\tab matches any single character
\par \tab +\tab matches one or more occurrences of the preceding character
\par \tab *\tab matches zero or more occurrences of the preceding character
\par \tab ?\tab matches exactly zero or one occurrences of the preceding character
\par \tab ^\tab matches beginning of line
\par \tab $\tab matches end of line
\par \tab [ and ]\tab define a list of characters which can match present character
\par \tab\tab between the brackets can be placed either a list of letters, any of which matches the current character, or the name of a character class which is matched against the current character.  The format for a character class is:
\par 
\par \tab [:classname:]
\par 
\par \pard\fi-680\li680\ri1460\tx680\tx1640\tab for example [[:alpha:]] matches any letter.  The class names follow the macros found in ctype.h:  the following class names are valid:
\par \pard\fi-1640\li1640\ri1480\tx700\tx1660 
\par \pard\fi-3360\li3360\ri1460\tx1640\tx3360\tab alpha\tab matches a letter
\par \tab upper\tab matches an upper case letter
\par \tab lower\tab matches a lower case letter
\par \tab digit\tab matches a digit
\par \tab alnum\tab matches a letter or digit
\par \tab xdigit\tab matches a hexadecimal digit
\par \tab space\tab matches any white space character, e.g. spaces,
\par \tab\tab newlines, tabs, form feeds 
\par \tab print\tab matches any printable character
\par \tab punct\tab matches punctuation characters
\par \tab graph\tab matches non-space printable characters
\par \tab cntrl\tab matches control characters
\par \tab blank\tab matches space and tab
\par 
\par \pard\fi-1640\li1640\ri1480\tx700\tx1660 
\par \tab |\tab alternator, an 'or' operator
\par \tab\\\tab escape character.  When it precedes a special character, it quotes that character.  When used with other characters it can cause special matching functions to occur
\par \tab 
\par The escape sequences are as follows:
\par 
\par \tab\\b\tab matches a word boundary
\par \tab\\B\tab matches inside a word
\par \tab\\w\tab matches word constituents
\par \tab\\W\tab matches non-word constituents
\par \tab\\<\tab matches beginning of word
\par \tab\\>\tab matches end of word
\par \tab\\`\tab matches beginning of buffer
\par \tab\\'\tab matches end of buffer
\par \tab\\( and \\)\tab define a group to be indexed in back-reference and replace operations
\par \tab\\\{ and \\\}\tab interval operators
\par \tab\tab for example: "a\{2,4\} matches from 2 to 4 'a' characters.  less than two or greater than four characters don't match
\par \tab\\digit\tab back-reference operator... references a parenthesized group
\par 
\par In the replace field most of the above sequences have no effect.  The following sequences have special meaning:
\par 
\par \tab\\digit\tab reference a parenthesized group specified in the find window
\par \tab\\\tab quote the next character\f1 
\par }
820
IDH_SAVE_FILE_DIALOG
Save File Dialog
dialog;save file dialog;



Writing



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Save File Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 When exiting the IDE, the IDE will check if any files in the edit windows have been modified but not saved.  If so a dialog similar to the following is displayed to the following to give the option to choose files to save:
\par 
\par \pard\ri1480\qc\cf2\{bmc savefiles.png\}\cf0 
\par \pard\ri1480 
\par Clicking on the checkbox next to a file will select or deselect it.  If a save operation is performed, only selected files will be saved.  By default all files that have changed are marked as selected, and files that have not changed are not shown in this dialog box.
\par 
\par \b Save\b0  saves the contents of the selected files to disk.  If no files are selected, save does nothing and the dialog closes.
\par 
\par \b Select All\b0  marks all files as selected.
\par 
\par \b Deselect\b0  all marks all files as unselected.
\par 
\par \b Cancel\b0  closes the dialog and aborts the exit.
\par 
\par \b Help\b0  displays this text.
\par 
\par }
830
IDH_RELOAD_FILE_DIALOG
Reload File Dialog
reload file dialog;



Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1480\cf1\b\fs32 Reload File Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 When the IDE is brought to the foreground, it will check if the contents of any of the files in the edit windows have been changed outside the source editor, for example in another editor or as a result of running a program that writes to the file.  If any files have changed and potentially need to be reloaded, a dialog similar to the following is displayed:
\par 
\par \pard\ri1480\qc\cf2\{bmc reloadfiles.png\}\cf0 
\par \pard\ri1480 
\par Clicking on the checkbox next to a file will select or deselect it.  If a reload operation is performed, only selected files will be reloaded.  By default all files that have changed are marked as selected, and files that have not changed are not shown in this dialog box.
\par 
\par \b Reload\b0  reloads the contents of the selected files from disk.  If no files are selected, reload does nothing and the dialog closes.
\par 
\par \b Select All\b0  marks all files as selected.
\par 
\par \b Deselect\b0  all marks all files as unselected.
\par 
\par \b Cancel\b0  closes the dialog without reloading any files
\par 
\par \b Help\b0  displays this text.
\par 
\par \b 
\par }
850
Scribble850
Hot Keys
hot keys;keys;



Writing



FALSE
113
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\ri1500\cf1\b\fs32 Hot Keys\cf0\b0\f1\fs20 
\par 
\par \f0 Many of the IDE functions may be performed with alternate key sequences.  This page gives a list of the key sequences and what they do.
\par 
\par In general, the following key sequences are always in effect:
\par 
\par \pard\ri1500\tx2760\tx2760\tx2760\b CTL-F\b0\tab Open the \cf2\strike Find/Replace Dialog\cf3\strike0\{linkID=100\}\cf0  to search for text
\par \b CTL-G\b0\tab Open the \cf2\strike Goto Line Dialog\cf3\strike0\{linkID=340\}\cf0  to position text in the current edit window to a specific line
\par \b CTL-H\b0\tab Open the Find/Replace Dialog to search for and replace text
\par \b F1\b0\tab The index page of this help file
\par \b F2\b0\tab Position to the next bookmark
\par \b SHIFT-F2\b0\tab Position to the previous bookmark
\par \b CTL-F2\tab\b0 Open the \cf2\strike Bookmark Dialog\cf3\strike0\{linkID=390\}\cf0  to select a bookmark to position to
\par \b ALT-F2\b0\tab Toggles bookmark status for the current line
\par \b F3\b0\tab Search for text again (find next)
\par \b SHIFT-F4\b0\tab Tiles the windows horizontally
\par \b CTL-F4\b0\tab Tiles the windows vertically
\par \b F5\b0\tab Start the debugger and/or run the program
\par \b SHIFT-F5\b0\tab Stop the debugger
\par \b CTL-F5\b0\tab Run the program until control flow reaches the selected line in the current editor window
\par \b ALT-F5\b0\tab Stop the program
\par \b F7\b0\tab Build the parts of all projects which are not up to date
\par \b SHIFT-F7\b0\tab Build the parts of the active project which are not up to date
\par \b CTL-F7\b0\tab Compile file in current window
\par \b F9\b0\tab Toggle break point at the selected line in the current editor window
\par \b F10\b0\tab Step over the current line
\par \b SHIFT-F10\b0\tab Run to cursor
\par \b F11\b0\tab Step into the current line
\par \b ALT-F11\b0\tab Step out of the current function
\par \b F12\b0\tab Browse to the definition of the selected word 
\par \b SHIFT-F12\b0\tab Browse back to the previous text position
\par 
\par \pard\fi-2800\li2800\ri1500\tx660\tx2800 In addition the following sequences have special meaning in the \cf2\strike Editor Window\cf3\strike0\{linkID=50\}\cf0 :
\par 
\par \pard\fi-2740\li2740\ri1500\tx2760\b ESC\b0\tab Close the find window or a code completion window
\par \b SHIFT-CTL-#\b0\tab # is a digit from 1 - 9.   Mark a position in the text
\par \b CTL-#\b0\tab # is a digit from 1 - 9.   Goto a previously marked position in the text
\par \b CTL-C\b0\tab Copy selection to clipboard
\par \b CTL-L\b0\tab Page Break
\par \b CTL-R\b0\tab Redo the last undone operation
\par \b CTL-S\b0\tab Save the file in the current edit window
\par \b CTL-T\b0\tab Delete word to right of cursor
\par \b CTL-V\b0\tab Paste clipboard to cursor position.  This overwrites the selection if there is one
\par \b CTL-X\b0\tab Copy the selection to the clipboard, and cut it from the text
\par \b CTL-Y\b0\tab Delete line cursor is on
\par \b CTL-Z\b0\tab Undo the last operation
\par \b ALT-\{\b0\tab Navigate to beginning of block when cursor is on a '\}'\b 
\par ALT-\}\b0\tab Navigate to end of block when cursor is on a '\{'
\par \b CTL-[ \b0                                      Open full-size edit window
\par \b CTL-] \b0                                      Close full-size edit window
\par \b CTL-|\b0\tab Place the line the cursor is at in the center of the window (vertical center)
\par \b SHIFT-F1\tab\b0 Show run time library help for the word under the cursor.
\par \b CTL-F1\b0\tab Show \cf2\strike WIN32 help\cf3\strike0\{linkID=555\}\cf0  for the word under the cursor
\par \b CTL-F7\b0\tab Compile the file in the source window
\par \b BACKSPACE\b0\tab Delete the character preceding the cursor
\par \b DELETE\b0\tab Delete the character at the cursor
\par \b END\b0\tab Move the cursor to the end of the line
\par \b CTL-END\b0\tab Move the cursor to the end of the file
\par \b HOME\b0\tab Move the cursor to the beginning of the line
\par \b CTL-HOME\b0\tab Move the cursor to the beginning of the file
\par \b INSERT\b0\tab toggles the insert mode between INSERT and OVERWRITE
\par \b PAGE DOWN\b0\tab Move the cursor down one page
\par \b PAGE UP\b0\tab Move the cursor up one page
\par \b TAB\b0\tab Insert a tab character, or indent text if it is selected.  Depending on the \cf2\strike Editor Formatting Configuration\cf3\strike0\{linkID=510\}\cf0 , the tab may be replaced by enough spaces to reach the next tab position.
\par \b SHIFT-TAB\b0\tab Delete a tab or spaces to the left of the cursor, or unindent if text is selected\f1 
\par \b\f0 DOWN ARROW\b0\tab Navigate downward one line
\par \b UP ARROW\b0\tab Navigate upward one line
\par \b LEFT ARROW\b0\tab Navigate left one character
\par \b RIGHT ARROW\b0\tab Navigate right one character
\par \b CTL-RIGHT ARROW\b0  \tab Navigate right one word
\par \b CTL-LEFT ARROW \b0\tab Navigate left one word
\par 
\par \pard\fi-2800\li2800\ri1500\tx660\tx2800 The following keys have special meaning in the \cf2\strike Workarea Window\cf3\strike0\{linkID=190\}\cf0 :
\par 
\par \pard\fi-2780\li2780\ri1500\tx2780\b DOWN ARROW\b0\tab Navigate downward one line
\par \b UP ARROW\b0\tab Navigate upward one line
\par \b LEFT ARROW\b0\tab Navigate left one character
\par \b RIGHT ARROW\b0\tab Navigate right one character
\par \b INSERT\b0\tab If the selection is on a project, open the \cf2\strike Open File Dialog\cf3\strike0\{linkID=350\}\cf0  and prompt for a new project to insert prior to this one.  Otherwise open the Open File Dialog and prompt for new files to insert in the selected project.
\par \b CTL-INSERT  \b0\tab Expand a node
\par \b DELETE\b0\tab If the selection is on a project, a prompt will appear asking whether to remove it from the workarea.  Accepting it will remove the project.  If the selection is on a file, the file will be removed from the workarea.
\par \b CTL-DELETE \b0\tab Collapse a node
\par \b ENTER\b0  \tab open the file corresponding to the node
\par 
\par \pard\fi-2800\li2800\ri1500\tx660\tx2800 The following keys have special meaning in the \cf2\strike Assembly Window\cf3\strike0\{linkID=670\}\cf0 :
\par 
\par \pard\fi-2780\li2780\ri1500\tx2760\b CTL-G\b0\tab Prompt for an address to position to
\par \b HOME\tab\b0 Navigate to the position the program is currently stopped at
\par \b DOWN ARROW\b0\tab Navigate downward one line
\par \b UP ARROW\b0\tab Navigate upward one line
\par \b LEFT ARROW\b0\tab Navigate left one character
\par \b RIGHT ARROW\b0\tab Navigate right one character
\par \b PAGE-UP\b0\tab Navigate upward one page
\par \b PAGE-DOWN\b0\tab Navigate downward one page
\par 
\par \pard\fi-2800\li2800\ri1500\tx660\tx2800 The following keys have special meaning in the \cf2\strike Memory Window\cf3\strike0\{linkID=680\}\cf0 :
\par 
\par \pard\fi-2780\li2780\ri1500\tx2760\tx2780\b DOWN ARROW\b0\tab Navigate downward one line
\par \b UP ARROW\b0\tab Navigate upward one line
\par \b LEFT ARROW\b0\tab Navigate left one character
\par \b RIGHT ARROW\b0\tab Navigate right one character
\par \b PAGE-UP\b0\tab Navigate upward one page
\par \b PAGE-DOWN\b0\tab Navigate downward one page
\par \tab These keys allow navigation through the memory
\par \b 0-9, A-F\b0\tab These keys let you enter a new value.  The characters will turn blue until ENTER is pressed to accept the change
\par \b ENTER\tab\b0 Accept a new value and program it into memory
\par \b ESCAPE\b0\tab Restore the original value for a change that has not been accepted
\par 
\par Most of the debug windows allow use of \b CTL-C\b0  to copy text from the window onto the clipboard.
\par \pard\cf1\b\fs32 
\par }
860
IDH_TOOLBAR_SELECT_DIALOG
Customize Tools




Writing



FALSE
13
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Customize Tools\cf0\b0\f1\fs20 
\par \cf2\f0 
\par The Customize Tools Dialog is accessible from the \cf3\strike Tools Menu\cf4\strike0\{linkID=120\}.  \cf0 It \cf2 shows a list of toolbars.  The dialog appears as follows:
\par 
\par \pard\qc\cf4\{bmc customizetoolbar.png\}\cf2 
\par 
\par \pard Here each of the six toolbars is listed, with a checkbox to indicate whether the toolbar is selected or deselected.  Selected toolbars appear on the display; deselected toolbars do not.
\par 
\par \cf0 Pressing the Customize button will open the \cf3\strike Toolbar Customization\cf4\strike0\{linkID=460\}\cf0  dialog, which allows rearranging the buttons on the toolbar.
\par \f1 
\par }
880
IDH_BUILD_RULE_DIALOG
Build Rules




Writing



FALSE
9
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Build Rules\cf0\b0\f1\fs20 
\par 
\par \f0 The Build Rules dialog allows disabling internal build rules.  In a future version of the IDE it will also allow creating and editing new build rules.  It appears as follows:
\par 
\par \pard\qc\cf2\{bmc buildrules.png\}\cf0 
\par \pard\f1 
\par }
890
IDH_DATA_BREAKPOINTS_DIALOG
Data Breakpoint
breakpoints;



Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green0\blue0;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Data Breakpoint\cf0\b0\f1\fs20 
\par 
\par \cf2\f0 The Data Breakpoints Dialog is used to set special microprocessor breakpoints that can be triggered on data access.  It appears as follows:
\par 
\par \pard\qc\cf3\{bmc databreakpoint.png\}\cf2 
\par \pard\f1 
\par \pard\ri1440\tx2100\f0 Data Breakpoints are general purpose breakpoints.  They can be set to trigger when any variable is accessed, including a complete structure or an array.  However, a data breakpoint slows down the program by a small amount.  In some cases where the limits of \cf4\strike Hardware Breakpoints\cf3\strike0\{linkID=730\}\cf2  can be tolerated, they are a better choice.
\par 
\par Each Data Breakpoint is simply an \cf4\strike Expression\cf3\strike0\{linkID=800\}\cf0  that determines the address the break point starts at.\cf2 
\par 
\par Data Breakpoints can be added and removed through the dialog.  Additionally, a checkbox to the left of the breakpoint allows the possibility of disabling breakpoint while keeping it in the list.
\par 
\par 
\par \pard\cf0\f1 
\par }
900
Scribble900
Jump List




Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Jump List\cf0\b0\f1\fs20 
\par 
\par \f0 The Jump List is a pair of combo boxes that appear just below the source file tabs, and above the edit windows which hold source files.  It allows selection of a type of global variable in one combo box;  when the type of the variable is selected the other combo box will be populated with variables of that type.  Selecting a variable from the second combo box causes the IDE to position the cursor to that function.
\par 
\par The possible types are:
\par 
\par \pard\tx1380\b\tab Global Functions
\par \tab Global Variables
\par \tab Global Types
\par \tab Preprocessor Macros
\par 
\par \pard\b0 Note that the Jump List depends on the program having previously been compiled with\cf2\strike  Browse Information\cf3\strike0\{linkID=430\}\cf0  enabled on the \cf2\strike Hints and Code Completion\cf3\strike0\{linkID=530\}\cf0  properties page.\f1 
\par }
910
IDH_CUSTOM_TOOLS_DIALOG
Custom Tools




Writing



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Custom Tools\cf0\b0\f1\fs20 
\par 
\par \f0 The Custom Tools dialog is accessible from the \cf2\strike Tools Menu\cf3\strike0\{linkID=120\}\cf0 .  It allows configuration of programs that can be run directly from the IDE.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc externaltools.png\}\cf0 
\par \pard 
\par This dialog shows a list of custom tools that have previously been defined.  Each tool has a checkmark, which indicates whether the tool should show up in the \cf2\strike Tools -> External Tools menu\cf3\strike0\{linkID=120\}.  \cf0 When a tool appears in the Tools menu, clicking on it will execute the program associated with the tool
\par 
\par The items will show up in the Tools menu in the same order they are displayed in this dialog, so there \b Move Up\b0  and \b Move Down \b0 options that move the selected item around.
\par 
\par \b Add\b0  creates a new item and opens the \cf2\strike Custom Tools Editor Dialog\cf3\strike0\{linkID=920\}\cf0  to set it up.
\par 
\par \b Edit\b0  opens the Custom Tools Editor dialog to edit the selected item.
\par 
\par \b Remove\b0  removes the selected item from the list of tools.\f1 
\par }
920
IDH_CUSTOM_TOOLS_EDITOR
Custom Tools Editor




Writing



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Custom Tools Editor\cf0\b0\f1\fs20 
\par 
\par \f0 The Custom Tools Editor is accessible from the \cf2\strike Tools Menu\cf3\strike0\{linkID=120\}\cf0 .  It allows modifying the name of the executable a \cf2\strike Custom Tool\cf3\strike0\{linkID=910\}\cf0  menu item will run, as well as parameters.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc externaltoolscustomize.png\}\cf0 
\par \pard\f1 
\par \f0 When a custom tool is selected from the \cf2\strike Tools Menu\cf3\strike0\{linkID=120\},\cf0  the IDE will use the information specified in this dialog to run the tool.
\par 
\par Several text boxes appear on the dialog:
\par 
\par \pard\tx1380\tx4220\b\tab Menu Name\b0\tab name of the item as it will appear on the menu\b 
\par \tab Command\b0\tab name of the program to run.  The PATH environment variable will be searched for the program.\b 
\par \tab Arguments\b0\tab any command-line arguments to be provided when the program runs.\b 
\par \tab Working Directory\b0\tab the directory the program will see if it calls a function like \f2 getpwd\f0  or \f2 GetCurrentDirectory\b\f0 
\par 
\par \pard\tx1400\b0 Additionally, the \b Open Command\b0  Window checkbox indicates whether the IDE should open an operating system command prompt to run the program in.\f1 
\par }
1000
Scribble1000
Resource Editor
resources;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Resource Editor\cf0\b0\f1\fs20 
\par 
\par \f0 OCIDE has a built-in resource editor.  This editor is capable of editing the following types of resources:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-840\li840\cf2\strike Accelerators\cf3\strike0\{linkID=1040\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Bitmaps\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Cursors\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Dialogs and Controls\cf3\strike0\{linkID=1060\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Icons\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Menus\cf3\strike0\{linkID=40\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}RCData\cf3\strike0\{linkID=1100\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}String Tables\cf3\strike0\{linkID=1120\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Version Data\cf3\strike0\{linkID=1140\}\cf0\f1 
\par \pard\fi-840\li840 
\par \pard\tx200\f0 Additionally, you can add Font and Message Table resources although you cannot edit them.
\par \f1 
\par \f0 See here for general remarks that may be useful in diagnosing problems loading resources.\f1 
\par 
\par \f0 A main \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0  shows a list of all available resources and allows easy addition and removal of resources.  Clicking on a resource will bring up the associated resource editor for that resource.\f1 
\par 
\par \f0 The resource editor interacts with a resource (.RC) file, where it stores the data in ASCII format.  It is capable of loading and saving most resource files, although there are a few limits.  For an existing resouce file, it will read in most types of resources even if the resource editor does not have native support for editing them.
\par 
\par 
\par }
1002
Scribble1002
Remarks about Resources
resource problems;resources;



Writing



FALSE
32
{\rtf1\ansi\ansicpg1251\deff0\deflang1049{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Times New Roman;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Remarks about Resources
\par \cf0\lang1049\b0\f1\fs20 
\par 
\par \lang1033\f0 The way resource files work, it is possible to reference a resource either by name, or by number.   For example for the resource statement:
\par 
\par \f2\fs24\tab ID_BMP bitmap "somefile.bmp"
\par \lang1049 
\par \f0\fs20 It is not immediately clear whether you would use a statement such as
\par 
\par \f2\fs24\tab HBITMAP myBmp = LoadBitmap(hDC, "ID_BMP");
\par 
\par \f0\fs20 or a statement such as:
\par 
\par \f2\fs24\tab HBITMAP myBmp = LoadBitmap(hDC, MAKEINTRESOURCE(ID_BMP));
\par \f0\fs20 
\par to actually load the bitmap into the program.   One of these is going to work, and the other isn't.
\par 
\par The rule is that if ID_BMP has not been defined anywhere, the resource compiler will compile a 'named' reference to the resource.   For example the first example would be the way to load it.  However, if it has been defined somewhere:
\par 
\par \f2\fs24\tab #define ID_BMP 100
\par \tab ID_BMP bitmap "somefile.bmp"
\par 
\par \f0\fs20 Then the resource compiler will compile a numeric reference to the resource, and one would have to use the second way of loading it.
\par 
\par Some projects have defined the resource name, and some have not.   So either method may be necessary for loading existing resources depending on the situation.   However, this IDE always adds a #define for resources, so the second method using MAKEINTRESOURCE is the preferred method for loading new resources.   This entire behavior is well-defined in windows and will be compatible across development environments, except in the case where a development environment adds a #define after the fact without indicating it.
\par 
\par This IDE will load and save resources identifiers in either format; if there is no #define one won't be added.   It is only when creating a new resource with the resource editor that one has to be aware to use the numeric reference via MAKEINTRESOURCE instead of the named reference.  But this behavior does make it possible to use the named version of resources.   Simply delete the associated #define after creating the resource, then reload the resource file.
\par 
\par \f2 
\par }
1005
Scribble1005
Resource Window




Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Resource Window\cf0\b0\f1\fs20 
\par 
\par \f0 The resource window appears in the same view as the \cf2\strike Workarea\cf3\strike0\{linkID=190\}\cf0 .  A tab at the bottom selects it, or selecting an .RC file will automatically select it.   Before the resource window will show resources, a resource file must be added to the project.  This can be done either by adding a new file with the \cf2\strike New File Dialog\cf3\strike0\{linkID=365\}\cf0 , and selecting its type to be .rc, or by bringing an existing resource file into the IDE with the \cf2\strike Open File Dialog\cf3\strike0\{linkID=350\}\cf0 .  Once the file is in the project, double clicking on it will load it into the IDE and its list of resources will automatically appear in the resource window.
\par 
\par The resource window appears as follows:
\par \f1 
\par \pard\qc\cf3\{bmc wresource.png\}
\par \pard\cf0 
\par 
\par \f0 In this case there is only one resource, a dialog with the resource identifieir ID_TEST.  The IDE will also create a file RESOURCE.H, which will have resource and control identifiers used by the program.  It can be #included into program modules to allow access to the resources.
\par 
\par At this point, right-clicking on the resource file name, which is called RCDemo.rc in this example, will bring up the \cf2\strike New Resouce Dialog\cf3\strike0\{linkID=1010\}\cf2 .\cf0   This allows selection of a new type of resource to add.  When the resource is added it will show up in the tree.  If there were no other resources of that type previously, OCIDE will insert a header name, for example 'Dialogs' or 'Accelerators'.  
\par 
\par A new resource will be given a default resource identifier, the resource identifier being the handle the C program uses to access the resource.  The resource identifier may be renamed either by double-clicking on it in the tree or by going to the \cf2\strike property window\cf3\strike0\{linkID=1026\} \cf0 and editing the resource identifier.  
\par 
\par Right-clicking on one of the headers will allow creation of new resources of the type specified by the header.  Right clicking on a resource will allow removing the resource.
\par 
\par Resource identifiers in OCIDE are always given a numeric value when created, however, OCIDE does allow you to use string versions of the resource identifiers simply be removing the associated #define from the RESOURCE.H file.
\par 
\par 
\par }
1010
Scribble1010
Add New Resource Dialog
dialog;



Writing



FALSE
24
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Add New Resource Dialog\cf0\b0\f1\fs20 
\par 
\par \f0 The add new resource dialog is accessible by selecting New Resource from the \cf2\strike Resource Menu\cf3\strike0\{linkID=1020\}\cf0 , or by clicking on an item in the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It allows selection of the type of new resource to add.  It appears similar to what follows:
\par 
\par \pard\qc\cf3\{bmc newresource.png\}\cf0 
\par \pard\f1 
\par \f0 It allows adding one of the following types of resources:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-840\li840\cf2\strike Accelerators\cf3\strike0\{linkID=1040\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Bitmaps\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Cursors\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Dialogs and Controls\cf3\strike0\{linkID=1060\}\cf0 
\par {\pntext\f2\'B7\tab}Fonts
\par \cf2\strike{\pntext\f2\'B7\tab}Icons\cf3\strike0\{linkID=1080\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Menus\cf3\strike0\{linkID=40\}\cf0 
\par {\pntext\f2\'B7\tab}Message Tables
\par \cf2\strike{\pntext\f2\'B7\tab}RCData\cf3\strike0\{linkID=1100\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}String Tables\cf3\strike0\{linkID=1120\}\cf0 
\par \cf2\strike{\pntext\f2\'B7\tab}Version Data\cf3\strike0\{linkID=1140\}\cf0\f1 
\par \pard 
\par \f0 Attempting to add a Bitmap, Cursor, Font, Icon, or Message Table will bring up the \cf2\strike Open File Dialog\cf3\strike0\{linkID=350\}\cf0  to ask for a file name.  In the case of images, if the file does not exist you will be asked if you want to create it.\f1 
\par }
1020
Scribble1020
Resource Menu
menu;



Writing



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Resource Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The resource menu looks as follows:
\par 
\par \pard\qc\cf2\{bmc mresdialoggrid.png\}      \{bmc mresimageflip.png\}   \{bmc mresimagerotate.png\}\cf0 
\par \pard 
\par 
\par The first image shows the resource menu as it appears when a \cf3\strike dialog edit window\cf2\strike0\{linkID=1060\}\cf0  is selected.  The following options are available:
\par 
\par \b Set Creation Order \b0 the dialog editor will go into the 'Set Creation Order' mode.  In this mode, selecting the controls one by one will set the order the controls will be created.  This affects the order for tabbing through the windows, and it will affect visibility if one window overlaps another.\b 
\par 
\par Set Tab Stops\b0  the dialog editor will go into the 'Set Tab Stops' mode.  In this mode, clicking on each control will set it's 'Tab Stop' setting.  The 'Tab Stop' setting indicates whether hitting tab on the preceding control will cause this control to get focus.  Each control will be outlined in red if it has no tab stop setting, or blue if it does.\b 
\par 
\par Set Group Flags \b0 the dialog editor will go into the 'Set Group Flags' mode.  In this mode, clicking on each control will set it's 'Group Flag' setting.  The 'Group Flag' setting indicates whether this control will be in the same group as adjacent controls.  The group flag setting is typically used for things like a set of radio buttons, each of which excludes the others when pressed.  Each control will be outlined in red if it has no group flag setting, or blue if it does.\b 
\par 
\par 
\par Grid Spacing\b0  this sets the density of the grid, when it is shown\b 
\par 
\par Show Grid \b0 show a grid to help align controls\b 
\par 
\par Snap To Grid\b0  when a control is created or moved, make sure its upper right corner aligns with the grid.\b 
\par 
\par 
\par \b0 In the second and third images the menu appears as it would when an image edit window for a bitmap, cursor, or icon is selected.  The following basic options are available:
\par 
\par \b Flip Selection
\par \tab Horizontally\b0  flips the selection horizontally\b 
\par \tab Vertically\b0  flips the selection vertically\b 
\par 
\par Rotate Selection
\par \tab 90 degrees clockwise\b0  rotates the selection 90 degrees clockwise\b 
\par \tab 90 degress counterclockwise\b0  rotates the selection 90 degrees counterclockwise\b 
\par \tab 180 degrees\b0  rotates the selection 180 degrees\b 
\par 
\par Clear Image\b0  set the image to the background color for bitmaps, or to see through for icons and cursors\b 
\par 
\par Clear Selection\b0  set the selection area to the background color for bitmaps, or to the see through color for icons and cursors\b\f1 
\par }
1026
Scribble1026
Resource Properties
properties;



Writing



FALSE
11
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Resource Properties
\par \cf0\b0\f1\fs20 
\par \f0 The resource Properties window is in the same view as the Dialog Control Toolbox.  It is accessible by selecting \b Property Window\b0  from the \cf2\strike View Menu\cf3\strike0\{linkID=180\}\cf0 .\f1 
\par \f0 Each type of resource has a properties window, which allows setting various attributes of the resource.  This can include things such as a title for dialogs, fonts to use, and various other attributes of resources or controls.  While it is beyond the scope of this documentation to talk about every property that may be edited in depth, an example for a dialog resource is shown below.\f1 
\par 
\par \pard\qc\cf3\{bmc wproperties.png\}\cf0\strike 
\par \pard\strike0 
\par \f0 The one property that needs explaining is the 'Extended' properly.  This property appears for menus and dialogs.  For a menu, it changes the resource from a MENU resource into a MENUEX resource, and for a dialog it changes the resource from a DIALOG resource to a DIALOGEX resource.\f1 
\par }
1040
Scribble1040
Accelerators Window
editor window;



Writing



FALSE
9
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Accelerators Window\cf0\b0\f1\fs20 
\par 
\par \f0 The accelerator edit window is accessible by clinking on an accelerator resource in the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It allows you to add keys to an accelerator table.  An accelerator table is a set of keyboard shortcuts which can be used in place of menus or buttons.  Each accelerator has an associated menu identifier, which is usually parsed and passed to the program's main window.  It appears similar to what follows:
\par \pard\qc\cf3\f1\{bmc waccelerator.png\}\cf0 
\par \pard\f0 
\par Here we have defined CTRL-A, PAGE HOME, CTRL-HOME, and SHIFT-RIGHT.  By using the \cf2\strike Accelerator Context Menu\cf3\strike0\{linkID=1050\}\cf0 , rows can be inserted or removed, and the key can be changed by pressing the key sequence to use.  And the various qualifiers such as CONTROL, ALT and SHIFT can be modified.  Usually, OCIDE will try to set these qualifiers according to the key sequence pressed, however, it may not be able to intercept the ALT key sequences so the ALT modifier must be set by hand.\f1 
\par }
1050
Scribble1050
Accelerators Context Menu
menu;



Writing



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Accelerators Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The accelerators context menu is accessible by right-clicking in the \cf2\strike Accelerator Edit Window\cf3\strike0\{linkID=1040\}\cf0 .    It allows inserting and deleting rows.  It also has an option that allows changing the key sequence.  It is invoked by right- clicking the accelerator edit window.  It appears as follows:
\par \f1 
\par \pard\qc\cf3\{bmc caccelerator.png\}
\par \pard\cf0\b\f0 Insert\b0  inserts a new row
\par 
\par \b Delete\b0  removes the selected row
\par 
\par \b Change Key\b0  allows changing the key sequence for a row.  When this is pressed a small box will appear with the word 'cancel'.  You can either press it to cancel the change, or press a key (including shift or control) to set a key sequence for this accelerator.
\par 
\par 
\par \f1 
\par 
\par }
1060
Scribble1060
Dialog Window
editor window;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Dialog Window\cf0\b0\f1\fs20 
\par 
\par \f0 The dialog edit window is accessible by selecting a dialog resource in the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It allows editing of dialogs.  Typically, a control will be dragged from the \cf2\strike Dialog Control Toolbox\cf3\strike0\{linkID=1065\}\cf0 , or dragged/resized while on the dialog.  Extended properties of the control can be set in the \cf2\strike Resource Properties\cf3\strike0\{linkID=1026\}\cf0  window after clicking on the control to select it.  Actions taken from either the \cf2\strike Resource Menu\cf3\strike0\{linkID=1020\}\cf0  or the \cf2\strike Dialog Context Menu\cf3\strike0\{linkID=1070\}\cf0  can effect changes to the dialog.  The dialog edit window looks similar to what follows when a couple of controls have been dragged on to it.
\par 
\par \f1 
\par \pard\qc\cf3\{bmc wdialog.png\}
\par \cf0 
\par \pard\f0 Here no title has been set for the dialog.  There is a combo box with some text to the left, and an edit box with the text to the left.  The common buttons 'ok' and 'cancel' have been added off to the right.
\par \f1 
\par }
1065
Scribble1065
Dialog Control Toolbox
dialog;



Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Dialog Control Toolbox\cf0\b0\f1\fs20 
\par 
\par \f0 The dialog control toolbox is in the same view as the \cf2\strike Properties Window\cf3\strike0\{linkID=1026\}\cf0 .  It shows a list of basic WIN32 controls that may be added to a dialog via the editor.  Dragging a dialog from the control toolbox to an active \cf2\strike Dialog Edit Window\cf3\strike0\{linkID=1060\}\cf0  adds a new instance of the control to the dialog.
\par \f1 
\par \pard\qc\cf3\{bmc wtoolbox.png\}\cf0 
\par \pard\f0 The types of controls OCIDE understands include:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 Push Button
\par {\pntext\f2\'B7\tab}Default Push Button
\par {\pntext\f2\'B7\tab}Check Box
\par {\pntext\f2\'B7\tab}Radio Button
\par {\pntext\f2\'B7\tab}Static Text
\par {\pntext\f2\'B7\tab}Icon
\par {\pntext\f2\'B7\tab}Text Editor
\par {\pntext\f2\'B7\tab}Combo Box
\par {\pntext\f2\'B7\tab}List Box
\par {\pntext\f2\'B7\tab}Group Box
\par {\pntext\f2\'B7\tab}Frame
\par {\pntext\f2\'B7\tab}Rectangle
\par {\pntext\f2\'B7\tab}Etched lines
\par {\pntext\f2\'B7\tab}Horizontal Scrollbar
\par {\pntext\f2\'B7\tab}Vertical Scrollbar\f1 
\par }
1070
Scribble1070
Dialog Context Menu
menu;



Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Dialog Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The dialog context menu is accessible by right-clicking in the \cf2\strike Dialog Editor Window\cf3\strike0\{linkID=1060\}\cf0 .  Many of these items are similar to items on the \cf2\strike Resource Menu\cf3\strike0\{linkID=1020\}\cf0 .  It appears as follows.
\par \f1 
\par \pard\qc\cf3\{bmc cdialog.png\}
\par \pard\cf0 
\par \b\f0 Set Creation Order \b0 the dialog editor will go into the 'Set Creation Order' mode.  In this mode, selecting the controls one by one will set the order the controls will be created.  This affects the order for tabbing through the windows, and it will affect visibility if one window overlaps another.\b 
\par 
\par Set Tab Stops\b0  the dialog editor will go into the 'Set Tab Stops' mode.  In this mode, clicking on each control will set it's 'Tab Stop' setting.  The 'Tab Stop' setting indicates whether hitting tab on the preceding control will cause this control to get focus.  Each control will be outlined in red if it has no tab stop setting, or blue if it does.\b 
\par 
\par Set Group Flags \b0 the dialog editor will go into the 'Set Group Flags' mode.  In this mode, clicking on each control will set it's 'Group Flag' setting.  The 'Group Flag' setting indicates whether this control will be in the same group as adjacent controls.  The group flag setting is typically used for things like a set of radio buttons, each of which excludes the others when pressed.  Each control will be outlined in red if it has no group flag setting, or blue if it does.\b 
\par 
\par 
\par Grid Spacing\b0  this sets the density of the grid, when it is shown\b 
\par 
\par Show Grid \b0 show a grid to help align controls\b 
\par 
\par Snap To Grid\b0  when a control is created or moved, make sure its upper right corner aligns with the grid.\b 
\par 
\par Delete\b0  removes the selected control from the dialog.\f1 
\par }
1080
Scribble1080
Image Window
editor window;



Writing



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Image window\cf0\b0\f1\fs20 
\par 
\par \f0 The image editor window is accessible by selecting an image resource in the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  This is a similar editor to older paint programs.  The image editor window has a \cf2\strike context menu\cf3\strike0\{linkID=1090\}\cf0  similar to the context menu seen on the \cf2\strike Resource Menu\cf3\strike0\{linkID=1020\}\cf0 , which allows some basic functionality.  The Image editor window appears as follows:
\par \f1 
\par \pard\qc\cf3\{bmc wicon.png\}
\par \cf0 
\par \pard\f0 The image editor window has a command palette to the left, which allows selection of common drawing functions such as selecting parts of an image, or drawing lines or drawing with brushes.
\par 
\par To the right of that is the drawing palette.  Here is where images are drawn
\par 
\par Below the image editor command palette is a secondary command palette that allows fine tuning of the command selection, for example selecting the width of a line or the combination of line and fill that will be drawn for a rectangle.
\par 
\par To the right of the secondary palette is a small diagonal color view which shows the colors that are currently selected.  The upper left corner shows the color that will be selected by pressing the left mouse button, and the lower right corner shows the color that will be selected by pressing the right mouse button.
\par 
\par For cursors and icons, there will be a blue box which allows selection of the 'transparent' color.  Left or Right clicking on it will make it the selected color for that mouse button.  For bitmaps, this is not there.
\par 
\par To the right of the transparent color selection or the diagonal color view is the palette.  Left or Right clicking on a palette item will make it the selected color for that mouse button.  Also, double clicking on a palette item will bring up the \cf2\strike Color Dialog\cf3\strike0\{linkID=470\}\cf0 , allowing the color of that palette item to be changed.
\par 
\par For cursors and icons, there will be a small tab bar below the drawing palette.  This allows selection of which item from a group of items is being viewed or edited.
\par 
\par }
1090
Scribble1090
Image Context Menu
menu;



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Image Context Menu\cf0\b0\f1\fs20 
\par 
\par \f0 The image context menu is accessible by right-clicking on the \cf2\strike Image Edit Window\cf3\strike0\{linkID=1080\}\cf0 .  It holds basic functionality for editing the image, similar to what is found on the \cf2\strike Resource Menu\cf3\strike0\{linkID=1020\}\cf0 .  It appears as follows:
\par \f1 
\par \pard\qc\cf3\{bmc ciconinsertimage.png\}
\par \pard\cf0 
\par \b\f0 Flip Selection
\par \tab Horizontally\b0  flips the selection horizontally\b 
\par \tab Vertically\b0  flips the selection vertically\b 
\par 
\par Rotate Selection
\par \tab 90 degrees clockwise\b0  rotates the selection 90 degrees clockwise\b 
\par \tab 90 degress counterclockwise\b0  rotates the selection 90 degrees counterclockwise\b 
\par \tab 180 degrees\b0  rotates the selection 180 degrees\b 
\par 
\par Clear Image\b0  set the image to the background color for bitmaps, or to see through for icons and cursors\b 
\par 
\par Clear Selection\b0  set the selection area to the background color for bitmaps, or to the see through color for icons and cursors\b\f1 
\par \b0 
\par \b\f0 Insert Image\b0  allows an image of a different size or color format to be inserted.  In the menu items, the first number is width, the second number is height, and the third number is the number of colors it can have.  So 32x32x16 is 32 wide, 32 high, and 16 colors.
\par 
\par \b Remove Image\b0  removes one the selected image.  However, there must always be at least one image.
\par \f1 
\par }
1100
Scribble1100
RCData Window
editor window;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset2 Symbol;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 RCData Window\cf0\b0\f1\fs20 
\par 
\par \f0 The RCData edit window is accessible by selecting an RCData resource from the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It appears similar to what follows:\f1 
\par 
\par \pard\qc\cf3\{bmc wrcdata.png\}\cf0 
\par \pard 
\par \f0 This window allows adding arbitrary data to a resource.  The types of data is selected in the left column, and the data is presented in the right column.  Valid data types are:
\par 
\par \pard{\pntext\f2\'B7\tab}{\*\pn\pnlvlblt\pnf2\pnindent0{\pntxtb\'B7}}\fi-200\li200\tx200 DWORD (4 bytes)
\par {\pntext\f2\'B7\tab}WORD (2 bytes)
\par {\pntext\f2\'B7\tab}ASCII text
\par \pard\tx200 
\par \pard At present the RCData window will not allow editing Unicode text.
\par }
1110
Scribble1110
RCData Context Menu
menu;



Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 RCData Context Menu
\par \cf0\b0\fs20 
\par The RCData context menu is accessible from the \cf2\strike RCData Edit Window\cf3\strike0\{linkID=1100\}\cf0 .  It allows for inserting and removing rows.  It appears similar to what follows:
\par \cf1\b\fs32 
\par \cf0\b0\f1\fs20 
\par \pard\qc\cf3\{bmc crcdata.png\}
\par \pard\cf0 
\par \b\f0 Insert\b0  inserts a new row
\par 
\par \b Delete\b0  removes the selected row
\par 
\par \f1 
\par }
1120
Scribble1120
String Table Window
editor window;



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 String Table Window\cf0\b0\f1\fs20 
\par \cf2 
\par \cf0\f0 The String Table edit window is accessible by selecting a String Table resource from the \cf3\strike Resource Window\cf2\strike0\{linkID=1005\}\cf0 .  It has a related \cf3\strike context menu\cf2\strike0\{linkID=1130\}\cf0  that allows inserting and deleting new strings.  It appears similar to what follows:
\par \cf2\f1 
\par \pard\qc\b\f0\{bmc wstring.png\}
\par \pard\b0\f1 
\par \cf0\f0 Here the left-hand column shows the resource identifier of the string, and the right-hand column shows the string.
\par 
\par \cf2\f1 
\par }
1130
Scribble1130
String Table Context Menu
menu;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 String Table Context Menu\cf0\b0\f1\fs20 
\par \f0 
\par The String Table context menu is accessible from the \cf2\strike String Table Edit Window\cf3\strike0\{linkID=1120\}\cf0 .  It has menu items for inserting and removing rows.  It appears similar to what follows:
\par \cf1\b\fs32 
\par \cf0\b0\f1\fs20 
\par \pard\qc\cf3\{bmc c\f0 string\f1 .png\}
\par \pard\cf0 
\par \b\f0 Insert\b0  inserts a new row
\par 
\par \b Delete\b0  removes the selected row
\par 
\par \f1 
\par 
\par }
1140
Scribble1140
Version Data Window
editor window;



Writing



FALSE
10
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Version Data Window
\par \cf0\b0\fs20 
\par The Version Data Window is accessible by selecting the Version resource from the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It allows editing of version information for the program or DLL.  This information can often be displayed by the operating system.  It has a related \cf2\strike context window\cf3\strike0\{linkID=1145\}\cf0  for inserting or removing string-related version information.  It appears as follows:
\par \cf1\b\fs32 
\par \pard\qc\cf3\fs20\{bmc wversion.png\}\cf0\b0\f1 
\par \pard 
\par \f0 Note that unlike other editor windows, this window comes pre-populated and typically it will just be a matter of changing the information to match your program.\f1 
\par }
1145
Scribble1145
Version Data Context Menu
menu;



Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green0\blue0;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Version Data Context Menu\cf2\b0\f1\fs20 
\par \cf0 
\par \f0 The Version Data context menu is accessible by right-clicking on the \cf3\strike Version Data Edit Window\cf2\strike0\{linkID=1140\}\cf0 .  It allows inserting and deleting string data.  It appears similar to what follows:\cf1\b\fs32 
\par \cf0\b0\f1\fs20 
\par \pard\qc\cf2\{bmc c\f0 version\f1 .png\}
\par \pard\cf0 
\par \b\f0 Insert\b0  inserts a new row.  This will only insert rows in the bottom or string section.
\par 
\par \b Delete\b0  removes the selected row.  This will only remove rows from the bottom or string section; the other information rows are fixed.
\par 
\par \f1 
\par 
\par }
1150
Scribble1150
Menu Window
editor window;



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Menu Window
\par \cf0\fs20 
\par \b0 The Menu Window is accessible by selecting a Menu resource from the \cf2\strike Resource Window\cf3\strike0\{linkID=1005\}\cf0 .  It allows editing menus.  It has an associated \cf2\strike context menu\cf3\strike0\{linkID=1160\}\cf0 , which is used for inserting or deleting menu items.  It appears similar to what follows:
\par \f1 
\par 
\par \pard\qc\cf3\b\f0\{bmc wmenu.png\}
\par \pard\cf0\b0\f1 
\par \f0 Here, the WORKAREA menu item has been clicked to drop down the WORKAREA menu.   Additionally, Close Work Area has been selected, as can be seen by the white box to the right of it.
\par 
\par The white boxes with arrows are the places where menu items can be inserted.  If a menu item is inserted to the right of another menu item it becomes an item on a sub menu.  The exception is the top row, where items to the right become new main menu selectors.
\par 
\par A menu item may also be inserted at the bottom of a menu, or in front of an existing menu item.  Additionally, a SEPARATOR may be inserted for grouping menu items.
\par \f1 
\par }
1160
Scribble1160
Menu Context Menu
menu;



Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Menu Context Menu
\par \cf0\b0\fs20 
\par The Menu Context Menu is accessible by right-clicking on a menu item in the \cf2\strike Menu Editor Window\cf3\strike0\{linkID=1150\}\cf0 .  It allows inserting and removing menu items.  It appears similar to what follows:
\par \cf1\b\fs32 
\par \pard\qc\cf3\fs20\{bmc cmenu.png\}\cf0\b0\f1 
\par \pard 
\par \b\f0 Insert\b0  inserts a new menu item
\par 
\par \b Insert Separator\b0  inserts a new separator
\par 
\par \b Delete\b0  removes a menu item
\par \f1 
\par }
0
0
0
184
1 General Information
2 Welcome=Scribble10
2 Getting Started=IDH_GETTING_STARTED
2 Using With NASM=Scribble840
2 GNU CopyLeft Statement=Scribble30
1 Bookmarks
2 Bookmarks=Scribble400
2 Bookmark Menu=Scribble160
2 Bookmark Toolbar=IDH_BOOKMARK_TOOLBAR
2 Bookmark Dialog=Scribble390
1 Breakpoints
2 Breakpoints=Scribble410
2 Breakpoints Window=Scribble415
3 Breakpoint Context Menu=Scribble417
2 Hardware Breakpoints Dialog=IDH_HARDWARE_BREAKPOINTS_DIALOG
2 Data Breakpoint Dialog=IDH_DATA_BREAKPOINTS_DIALOG
1 Browse Information
2 Browse information=Scribble430
2 Browse Menu=Scribble170
2 Jump List=Scribble900
2 Variable Usages Window=Scribble435
2 General Configuration=IDH_GENERAL_CONFIGURATION
1 Dialogs
2 Add To Watch Dialog=Scribble370
2 Add New Resource Dialog=Scribble1010
2 Bookmark Dialog=Scribble390
2 Browse To Dialog=Scribble380
2 Color Dialog=IDH_CHOOSE_COLOR_DIALOG
2 Custom Tools Dialog=IDH_TOOLBAR_SELECT_DIALOG
2 Custom Tools Editor Dialog=IDH_CUSTOM_TOOLS_EDITOR
2 Customize Tools Dialog=IDH_TOOLBAR_SELECT_DIALOG
2 Data Breakpoint Dialog=IDH_DATA_BREAKPOINTS_DIALOG
2 Find Dialog=Scribble310
2 Find in Files Dialog=Scribble280
2 Font Dialog=IDH_CHOOSE_FONT_DIALOG
2 General Properties Dialog=IDH_GENERAL_PROPERTIES_DIALOG
2 Goto Line Dialog=Scribble340
2 New File Dialog=IDH_NEW_FILE_DIALOG
2 New Project Dialog=IDH_NEW_PROJECT_DIALOG
2 New Work Area Dialog=IDH_NEW_WORKAREA_DIALOG
2 Open File Dialog=Scribble350
2 Project Dependencies Dialog=IDH_PROJECT_DEPENDENCIES_DIALOG
2 Project Properties Dialog=IDH_PROJECT_PROPERTIES_DIALOG
2 Reload File Dialog=IDH_RELOAD_FILE_DIALOG
2 Replace Dialog=Scribble320
2 Save File Dialog=IDH_SAVE_FILE_DIALOG
2 Select Window Dialog=Scribble330
2 Project Properties Dialog=IDH_PROJECT_PROPERTIES_DIALOG
2 Toolbar Customization Dialog=Scribble460
1 External Tools
2 Tools Menu=Scribble120
2 Custom Tools=IDH_CUSTOM_TOOLS_DIALOG
3 Custom Tools Editor=IDH_CUSTOM_TOOLS_EDITOR
2 Build Rules=IDH_BUILD_RULE_DIALOG
1 Editor
2 Editor Window=Scribble50
2 Editor Configuration=IDH_GENERAL_PROPERTIES_DIALOG
3 Basic Settings=Scribble490
3 Colors=Scribble500
3 Formatting=Scribble510
3 Backup Settings=Scribble520
3 Hints And Code Completion=Scribble530
3 Printer Settings=Scribble540
3 WIN32 Help Configuration=Scribble555
2 Edit Toolbar=IDH_EDIT_TOOLBAR
2 Edit Menu=Scribble130
2 Editor Context Menu=Scribble150
1 Help
2 Integrated Platform Help=Scribble420
2 Install Configuration=IDH_INSTALL_CONFIGURATION
2 Help Menu=Scribble260
1 Information Windows
2 Information Window=IDM_INFORMATION_WINDOW
2 Information Context Menu=Scribble290
2 Error List=Scribble295
1 Integrated Debugger
2 Integrated Debugger=Scribble300
2 Debug Target Properties=IDH_DEBUG_CONFIGURATION
2 Debug Toolbar=IDH_DEBUG_TOOLBAR
2 Debug Menu=Scribble310
2 Expressions=Scribble800
2 Debugging Windows=Scribble320
3 Hardware Breakpoints=IDH_HARDWARE_BREAKPOINTS_DIALOG
3 Data Breakpoints=IDH_DATA_BREAKPOINTS_DIALOG
3 Disassembly Window=IDH_ASSEMBLY_WINDOW
3 Memory Window=IDH_MEMORY_WINDOW
3 Register Window=IDH_REGISTER_WINDOW
3 Call Stack Window=IDH_STACK_WINDOW
3 Threads Window=IDH_THREAD_WINDOW
3 Watch Window=IDH_WATCH_WINDOW
3 Locals Window=Scribble655
1 Menus
2 Menus=Scribble40
2 Main Menu
3 File Menu=Scribble60
3 Edit Menu=Scribble130
3 View Menu=Scribble180
4 View Browse Menu=Scribble170
4 View Bookmarks Menu=Scribble160
3 Search Menu=Scribble90
3 Project Menu=Scribble110
3 Build Menu=Scribble250
3 Debug Menu=Scribble310
3 Tools Menu=Scribble120
3 Resource Menu=Scribble1020
3 Window Menu=Scribble140
3 Help Menu=Scribble260
2 Resource Editor Context Menus
3 Accelerators Context Menu=Scribble1050
3 Dialog Context Menu=Scribble1070
3 Image Context Menu=Scribble1090
3 Menu Context Menu=Scribble1160
3 RCData Context Menu=Scribble1110
3 String Table Context Menu=Scribble1130
3 Version Datta Context Menu=Scribble1145
2 Other Context Menus
3 Breakpoint Context Menu=Scribble417
3 Client Context Menu=Scribble195
3 Editor Context Menu=Scribble150
3 Information Context Menu=Scribble290
3 Memory Context Menu=Scribble690
3 Watch Context Menu=Scribble660
3 Workarea Context Menu=Scribble210
3 Folder Context Menu=Scribble220
3 Project Context Menu=Scribble230
3 File Context Menu=Scribble240
1 Resource Editor
2 Resource Editor=Scribble1000
3 Resource Menu=Scribble1020
2 Resource Window=Scribble1005
2 Add New Resource Dialog=Scribble1010
2 Resource Editor Windows
3 Resource Properties=Scribble1026
3 Accelerators Window=Scribble1040
4 Accelerators Context Menu=Scribble1050
3 Dialog Window=Scribble1060
4 Dialog Control Toolbox=Scribble1065
4 Dialog Context Menu=Scribble1070
3 Image Window=Scribble1080
4 Image Context Menu=Scribble1090
3 Menu Window=Scribble1150
4 Menu Context Menu=Scribble1160
3 RCData Window=Scribble1100
4 RCData Context Menu=Scribble1110
3 String Table Window=Scribble1120
4 String Table Context Menu=Scribble1130
3 Version Data Window=Scribble1140
4 Version Data Context Menu=Scribble1145
1 Workarea Window
2 Workarea Window=Scribble190
2 Build Toolbar=IDH_BUILD_TOOLBAR
2 Build Type Toolbar=IDH_BUILD_TYPE_TOOLBAR
2 Build Menu=Scribble250
2 Workarea Context Menu=Scribble210
2 New Work Area Dialog=IDH_NEW_WORKAREA_DIALOG
2 New Project Dialog=IDH_NEW_PROJECT_DIALOG
2 New File Dialog=IDH_NEW_FILE_DIALOG
2 Generated Make File=Scribble270
2 Project Properties Dialog=IDH_PROJECT_PROPERTIES_DIALOG
3 Profiles=Scribble585
4 New Profile Dialog=Scribble587
4 Select Profile Dialog=IDH_SELECT_PROFILE_DIALOG
3 Debug Project Config Properties=Scribble570
3 General Project Configuration Properties=Scribble560
3 Compiler Properties=Scribble590
3 Assembler Properties=Scribble600
3 Resource Compiler Properties=Scribble610
3 Linker Properties=Scribble620
3 Librarian Properties=Scribble630
3 Custom Build Properties=Scribble640
1 Support Functions
2 Find and Replace=IDH_FIND_REPLACE_DIALOG
2 Hot Keys=Scribble850
2 Printing=Scribble440
2 Regular Expressions=Scribble810
1 Toolbars
2 Toolbars=Scribble450
2 Toolbar Customization=IDH_TOOLBAR_SELECT_DIALOG
2 Edit Toolbar=IDH_EDIT_TOOLBAR
2 Navigation Toolbar=IDH_NAV_TOOLBAR
2 Build Toolbar=IDH_BUILD_TOOLBAR
2 Debug Toolbar=IDH_DEBUG_TOOLBAR
2 Bookmark Toolbar=IDH_BOOKMARK_TOOLBAR
2 Thread Toolbar=IDH_THREAD_TOOLBAR
0
