# Vars ###################################
# Colors
RED		= \033[0;31m
YELLOW	= \033[0;33m
CYAN	= \033[0;36m
RST		= \033[0m
IT		= \e[3m
ITRST	= \e[0m

# Directories
PREFIX	= /usr/local
BINDIR	= $(PREFIX)/bin
DATADIR	= $(PREFIX)/share
ARTDIR	= $(DATADIR)/kimsay/portraits
TXTDIR	= $(DATADIR)/kimsay/dialog
MANDIR	= $(DATADIR)/man/man6

# Sources
SRC		= src/kimsay.cpp
SRCART	= $(wildcard art/*)
SRCTXT	= $(wildcard dialog/*.json)
SRCMAN	= man/man6/kimsay.6

# Destinies
BINARY	= $(BINDIR)/kimsay
DSTART	= $(addprefix $(ARTDIR)/, $(notdir $(SRCART)))
DSTTXT	= $(addprefix $(TXTDIR)/, $(notdir $(SRCTXT)))
DSTMAN	= $(MANDIR)/kimsay.6

# Compilation
CXX			= c++
CXXFLAGS	= -s -flto -o $(BINARY) -D FILEDIR=\"$(DATADIR)\"

# Rules ##################################
.PHONY: all
all:
	@printf "\n$(YELLOW)Run 'make install' to perform the installation.$(RST)"
	@printf "\n\nYou can customize the installation path with 'make install PREFIX=<<your path>>',\nwhich will install the executable in PREFIX/bin, the files in PREFIX/share and the manpage in PREFIX/share/man/man6.\nYou can further refine these paths by assigning 'BINDIR', 'DATADIR' and 'MANDIR' respectively.\n\n"

.PHONY: install
install: remLegacy $(BINARY) dirs art dialog manpage
	@printf "\nYou now have access to the wisdom of 57's finest. Don't let him down.\n\n"

.PHONY: remLegacy
remLegacy:
	@if [ "$(shell id -u)" -ne 0 ] && [ $(PREFIX) = '/usr/local' ]; then \
		printf "\n$(RED)Global installation requires sudo priviledges, run 'sudo make install'\nor change the installation prefix to install locally with 'make install PREFIX=~/.local'$(RST)\n\n"; \
		exit 1; \
	fi
	@if [ -x /usr/bin/kimsay ]; then \
		printf "\n$(YELLOW)Legacy kimsay installation found!$(RST)\nRemoving legacy install...\n"; \
		rm /usr/bin/kimsay; \
		printf "\tExecutable deleted (/usr/bin/kimsay)\n"; \
		rm -rf /usr/share/kimsay; \
		printf "\tFiles deleted (/usr/share/kimsay)\n"; \
		rm /usr/share/man/man6/kimsay.6; \
		printf "\tManpage deleted (rm /usr/share/man/man6/kimsay.6)\n"; \
	fi

$(BINARY): $(SRC)
	@printf "\nCompiling kimsay..."
	@$(CXX) $(CXXFLAGS) $(SRC)
	@printf "\n\t$(CYAN)Compilation done!$(RST)\n"

.PHONY: dirs
dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(ARTDIR)
	@mkdir -p $(TXTDIR)
	@mkdir -p $(MANDIR)

.PHONY: art
art: $(DSTART)
	@printf "\n$(CYAN)Art files created~$(RST)"

.PHONY: dialog
dialog: $(DSTTXT)
	@printf "\n$(CYAN)Dialog files created~$(RST)"

.PHONY: manpage
manpage: $(DSTMAN)
	@printf "\n$(CYAN)Manpage created~$(RST)\n"

$(DSTART): $(SRCART)
$(DSTTXT): $(SRCTXT)
$(DSTMAN): $(SRCMAN)
$(DATADIR)/%:
	@cp $< $@

.PHONY: uninstall
uninstall:
	@if [ "$(shell id -u)" -ne 0 ] && [ $(PREFIX) = '/usr/local' ]; then \
		printf "\n$(RED)Attempting to perform a global uninstall without sudo priviledges,\nrun 'sudo make uninstall' or change the installation prefix to\nuninstall locally with 'make uninstall PREFIX=~/.local'$(RST)\n\n"; \
		exit 1; \
	fi
	@rm $(BINARY)
	@rm -rf $(DATADIR)/kimsay
	@rm $(DSTMAN)