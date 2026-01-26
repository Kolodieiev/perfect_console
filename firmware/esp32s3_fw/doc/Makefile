SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build
DOXYBUILDDIR  = docs

PORT          ?= 8000

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile dox_xml dox_html clean-doxygen serve autobuild autobuild-full

dox_xml:
	@echo "Generating Doxygen XML for Sphinx..."
	doxygen Doxyfile_xml

clean-doxygen:
	rm -rf $(DOXYBUILDDIR)

clean: clean-doxygen
	@$(SPHINXBUILD) -M clean "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

build: dox_xml
	@echo "Starting sphinx-autobuild with Doxygen watch"
	sphinx-autobuild "$(SOURCEDIR)" "$(BUILDDIR)/html" \
		--open-browser \
		--delay 1 \
		--port 8000 \
		--ignore "$(SOURCEDIR)/classes/*" \
		--ignore "$(SOURCEDIR)/classes/**/*" \
		--watch ../src \
		--pre-build "doxygen Doxyfile_xml"

%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)