SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build
DOXYBUILDDIR  = docs
PORT          ?= 8000

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile dox_xml clean-doxygen clean build dev_doc

dox_xml:
	@echo "Generating Doxygen XML for Sphinx..."
	doxygen Doxyfile_xml

clean-doxygen:
	rm -rf $(DOXYBUILDDIR)
	rm -rf $(SOURCEDIR)/classes

clean: clean-doxygen
	@$(SPHINXBUILD) -M clean "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

build: dox_xml
	@echo "Starting full build with Doxygen watch..."
	sphinx-autobuild "$(SOURCEDIR)" "$(BUILDDIR)/html" \
		--open-browser \
		--delay 2 \
		--port $(PORT) \
		--ignore "$(SOURCEDIR)/classes/*" \
		--ignore "$(SOURCEDIR)/classes/**/*" \
		--watch ../src \
		--pre-build "doxygen Doxyfile_xml"

dev_doc:
	@echo "Starting light build (without API docs)..."
	sphinx-autobuild "$(SOURCEDIR)" "$(BUILDDIR)/html" \
		--open-browser \
		--delay 2 \
		--port $(PORT) \
		--ignore "$(BUILDDIR)" \
		--ignore "$(BUILDDIR)/**/*" \
		--ignore "$(DOXYBUILDDIR)" \
		--ignore "$(DOXYBUILDDIR)/**/*" \
		--ignore "$(SOURCEDIR)/classes" \
		--ignore "$(SOURCEDIR)/classes/**/*" \
		-D extensions="sphinx.ext.viewcode,myst_parser" \
		-D exclude_patterns="classes/**"

%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)