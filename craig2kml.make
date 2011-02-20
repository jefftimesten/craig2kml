# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug)
  OBJDIR     = obj/Debug
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/craig2kml
  DEFINES   += -DDEBUG
  INCLUDES  += -I/opt/local/include -I/opt/local/include/libxml2 -I/usr/local/include/tidy -I/usr/local/include/kml/base -I/usr/local/include/kml/convenience -I/usr/local/include/kml/dom -I/usr/local/include/kml/engine -I/usr/local/include/kml/regionator -I/usr/local/include/kml/third_party -I/usr/local/include/kml/xsd -I/usr/local/include/kml/third_party/boost_1_34_1 -I/usr/local/include/kml/third_party/boost_1_34_1/boost -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config -I/usr/local/include/kml/third_party/boost_1_34_1/boost/detail -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/compiler -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/no_tr1 -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/platform -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/stdlib
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -L/opt/local/lib -L/usr/local/lib -L/usr/lib
  LIBS      += -lxml2 -ltidy -lcurl -lz -lpthread -lm -lkmlbase -lkmlconvenience -lkmldom -lkmlengine
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  OBJDIR     = obj/Release
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/craig2kml
  DEFINES   += -DNDEBUG
  INCLUDES  += -I/opt/local/include -I/opt/local/include/libxml2 -I/usr/local/include/tidy -I/usr/local/include/kml/base -I/usr/local/include/kml/convenience -I/usr/local/include/kml/dom -I/usr/local/include/kml/engine -I/usr/local/include/kml/regionator -I/usr/local/include/kml/third_party -I/usr/local/include/kml/xsd -I/usr/local/include/kml/third_party/boost_1_34_1 -I/usr/local/include/kml/third_party/boost_1_34_1/boost -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config -I/usr/local/include/kml/third_party/boost_1_34_1/boost/detail -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/compiler -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/no_tr1 -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/platform -I/usr/local/include/kml/third_party/boost_1_34_1/boost/config/stdlib
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -Wl,-x -L/opt/local/lib -L/usr/local/lib -L/usr/lib
  LIBS      += -lxml2 -ltidy -lcurl -lz -lpthread -lm -lkmlbase -lkmlconvenience -lkmldom -lkmlengine
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/Craig2KML.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/Webpage.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking craig2kml
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning craig2kml
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/Craig2KML.o: src/Craig2KML.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Webpage.o: src/Webpage.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
