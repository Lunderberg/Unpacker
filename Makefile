.PHONY: clean all
.SECONDARY:
.SECONDEXPANSION:

# EDIT THIS SECTION

INCLUDES   = include
CPP        = g++
CFLAGS     = -Wall -Wextra -pedantic -std=c++11 -O3 -g
LINKFLAGS  =
SRC_SUFFIX = cxx

# EVERYTHING PAST HERE SHOULD WORK AUTOMATICALLY

COM_COLOR=\033[0;34m
OBJ_COLOR=\033[0;36m
OK_COLOR=\033[0;32m
ERROR_COLOR=\033[0;31m
WARN_COLOR=\033[0;33m
NO_COLOR=\033[m

LIBRARY_DIRS   := $(shell find libraries -type d -links 2)
LIBRARY_NAMES  := $(notdir $(LIBRARY_DIRS))
LIBRARY_OUTPUT := $(patsubst %,libraries/lib%.so,$(LIBRARY_NAMES))

INCLUDES  := $(addprefix -I,$(INCLUDES))
CFLAGS    += $(shell root-config --cflags)
CFLAGS    += -MMD $(INCLUDES)
LINKFLAGS += -Llibraries $(addprefix -l,$(LIBRARY_NAMES)) -Wl,-rpath,\$$ORIGIN/../libraries
LINKFLAGS += $(shell root-config --glibs) -lSpectrum

UTIL_O_FILES    := $(patsubst %.$(SRC_SUFFIX),build/%.o,$(wildcard utils/*.$(SRC_SUFFIX)))
SANDBOX_O_FILES := $(patsubst %.$(SRC_SUFFIX),build/%.o,$(wildcard sandbox/*.$(SRC_SUFFIX)))
EXE_O_FILES     := $(UTIL_O_FILES) $(SANDBOX_O_FILES)
EXECUTABLES     := $(patsubst %.o,bin/%,$(notdir $(EXE_O_FILES)))

run_and_test = @$(1) 2> $(2).log || touch $(2).error; \
                if test -e $(2).error; then \
                      echo -e "$(ERROR_COLOR)Error making $(2)$(NO_COLOR)" && \
                      cat $(2).log; \
                      rm -f $(2).log $(2).error; \
                      exit 1; \
                elif test -s $(2).log; then \
                      echo -e "$(WARN_COLOR)Warning making $(2)$(NO_COLOR)" && \
                      cat $(2).log; \
                fi; \
                rm -f $(2).log $(2).error

all: $(EXECUTABLES) $(LIBRARY_OUTPUT)
	@echo -e "$(OK_COLOR)Compilation successful$(NO_COLOR)"

bin/%: build/sandbox/%.o | $(LIBRARY_OUTPUT) bin
	@echo -e "$(COM_COLOR)Compiling $(OBJ_COLOR)$@ $(NO_COLOR)"
	$(call run_and_test, $(CPP) $< -o $@ $(LINKFLAGS), $<, $@)

bin/%: build/utils/%.o | $(LIBRARY_OUTPUT) bin
	@echo -e "$(COM_COLOR)Compiling $(OBJ_COLOR)$@ $(NO_COLOR)"
	$(call run_and_test, $(CPP) $< -o $@ $(LINKFLAGS), $<, $@)

bin:
	@mkdir -p $@

# Functions for determining the files included in a library.
# All src files in the library directory are included.
# If a LinkDef.h file is present in the library directory,
#    a dictionary file will also be generated and added to the library.
libdir          = $(shell find libraries -name $(1) -type d)
lib_src_files   = $(shell find $(call libdir,$(1)) -name "*.$(SRC_SUFFIX)")
lib_o_files     = $(patsubst %.$(SRC_SUFFIX),build/%.o,$(call lib_src_files,$(1)))
lib_linkdef     = $(wildcard $(call libdir,$(1))/LinkDef.h)
lib_dictionary  = $(patsubst %/LinkDef.h,build/%/Dictionary.o,$(call lib_linkdef,$(1)))

libraries/lib%.so: $$(call lib_o_files,%) $$(call lib_dictionary,%)
	@echo -e "$(COM_COLOR)Building  $(OBJ_COLOR)$@ $(NO_COLOR)"
	$(call run_and_test, $(CPP) -fPIC -shared -o $@ $^, $@)

build/%.o: %.$(SRC_SUFFIX)
	@echo -e "$(COM_COLOR)Compiling $(OBJ_COLOR)$@ $(NO_COLOR)"
	@mkdir -p $(dir $@)
	$(call run_and_test, $(CPP) -fPIC -c $< -o $@ $(CFLAGS), $@)

build/%/Dictionary.o: build/%/Dictionary.cc
	@echo -e "$(COM_COLOR)Compiling $(OBJ_COLOR)$@ $(NO_COLOR)"
	$(call run_and_test, $(CPP) -fPIC -c $< -o $@ $(CFLAGS), $@)


dict_header_files = $(subst //,,$(shell head $(1) -n 1))

build/%/Dictionary.cc: %/LinkDef.h
	@echo -e "$(COM_COLOR)Building  $(OBJ_COLOR)$@ $(NO_COLOR)"
	@mkdir -p $(dir $@)
	$(call run_and_test, rootcint -f $@ -c $(INCLUDES) $(ROOTCFLAGS) -I$(dir $<) $(call dict_header_files,$<) $(notdir $<), $@)

-include $(shell find build -name '*.d' 2> /dev/null)

clean:
	@echo "Cleaning up"
	@rm -rf build
	@rm -rf bin
	@rm -f $(LIBRARY_OUTPUT)