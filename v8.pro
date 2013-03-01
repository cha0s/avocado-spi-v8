TEMPLATE = subdirs

CONFIG += ordered

DEFINES += AVOCADO

SUBDIRS += deps $$system(find -maxdepth 1 -mindepth 1 -type d ! -name 'deps')
