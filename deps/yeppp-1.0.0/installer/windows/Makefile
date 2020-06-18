all: Yeppp-SDK.msi

Version.i:
	$(CC) /nologo /I$(YEPROOT)library/headers /P /FiVersion.i /DYEP_PREPROCESS_TO_XML /EP $(YEPROOT)/installer\windows\Version.c

Version.wxi: Version.i
	"$(YEPROOT)\installer\windows\Version.bat" Version.i Version.wxi

CHeaders.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\library\headers" -nologo -cg CHeaders -gg -g1 -sfrag -srd -dr Library.Headers -var "var.SourceDirectory" -out $@

FortranModules.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\bindings\fortran\sources" -nologo -cg FortranModules -gg -g1 -sfrag -srd -dr Bindings.Fortran.Sources -var "var.SourceDirectory" -out $@

CExamples.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\examples\c" -nologo -cg CExamples -gg -g1 -sfrag -srd -dr Examples.C -var "var.SourceDirectory" -out $@

CSharpExamples.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\examples\csharp\sources" -nologo -cg CSharpExamples -gg -g1 -sfrag -srd -dr Examples.CSharp.Sources -var "var.SourceDirectory" -out $@

JavaExamples.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\examples\java\sources" -nologo -cg JavaExamples -gg -g1 -sfrag -srd -dr Examples.Java -var "var.SourceDirectory" -out $@

FortranExamples.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\examples\fortran\sources" -nologo -cg FortranExamples -gg -g1 -sfrag -srd -dr Examples.Fortran -var "var.SourceDirectory" -out $@

CDocumentation.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\docs\c" -nologo -cg CDocumentation -gg -g1 -sfrag -srd -dr Documentation.C -var "var.SourceDirectory" -out $@

FortranDocumentation.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\docs\fortran" -nologo -cg FortranDocumentation -gg -g1 -sfrag -srd -dr Documentation.Fortran -var "var.SourceDirectory" -out $@

CSharpDocumentation.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\docs\cs" -nologo -cg CSharpDocumentation -gg -g1 -sfrag -srd -dr Documentation.CSharp -var "var.SourceDirectory" -out $@

JavaDocumentation.wxs:
	"$(WIX)\bin\heat" dir "$(YEPROOT)\docs\java" -nologo -cg JavaDocumentation -gg -g1 -sfrag -srd -dr Documentation.Java -var "var.SourceDirectory" -out $@

Yeppp-SDK.wixobj: Yeppp-SDK.wxs Version.wxi
	"$(WIX)\bin\candle" -nologo -out $@ Yeppp-SDK.wxs

CHeaders.wixobj: CHeaders.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\library\headers" -out $@ $**

FortranModules.wixobj: FortranModules.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\bindings\fortran\sources" -out $@ $**

CExamples.wixobj: CExamples.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\examples\c" -out $@ $**

CSharpExamples.wixobj: CSharpExamples.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\examples\csharp\sources" -out $@ $**

JavaExamples.wixobj: JavaExamples.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\examples\java\sources" -out $@ $**

FortranExamples.wixobj: FortranExamples.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\examples\fortran\sources" -out $@ $**

CDocumentation.wixobj: CDocumentation.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\docs\c" -out $@ $**

FortranDocumentation.wixobj: FortranDocumentation.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\docs\fortran" -out $@ $**

CSharpDocumentation.wixobj: CSharpDocumentation.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\docs\cs" -out $@ $**

JavaDocumentation.wixobj: JavaDocumentation.wxs
	"$(WIX)\bin\candle" -nologo "-dSourceDirectory=$(YEPROOT)\docs\java" -out $@ $**

Yeppp-SDK.msi: Yeppp-SDK.wixobj CHeaders.wixobj FortranModules.wixobj CExamples.wixobj CSharpExamples.wixobj JavaExamples.wixobj FortranExamples.wixobj CDocumentation.wixobj FortranDocumentation.wixobj CSharpDocumentation.wixobj JavaDocumentation.wixobj
	"$(WIX)\bin\light" -b "$(YEPROOT)\." -nologo -ext WixUIExtension -out $@ $**
