#!/bin/bash
chmod -R 755 /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}
export PG_VER=9.6
# export PGPORT=8442
export OS_BUILD=64
# export POSTGIS_MAJOR_VERSION=2
# export POSTGIS_MINOR_VERSION=2
# export POSTGIS_MICRO_VERSION=0dev
export PROJECTS=/var/lib/jenkins/workspace
export GEOS_VER=3.6
export GDAL_VER=2.2
export WEB_DIR=/var/www/postgis_stuff
export PGPATH=${PROJECTS}/pg/rel/pg${PG_VER}w${OS_BUILD}
export PATH="${PGPATH}/bin:$PATH"
export LD_LIBRARY_PATH="${PROJECTS}/gdal/rel-${GDAL_VER}w${OS_BUILD}/lib:${PROJECTS}/geos/rel-${GEOS_VER}w${OS_BUILD}/lib:${PGPATH}/lib"
./autogen.sh



POSTGIS_MAJOR_VERSION=`grep ^POSTGIS_MAJOR_VERSION Version.config | cut -d= -f2`
POSTGIS_MINOR_VERSION=`grep ^POSTGIS_MINOR_VERSION Version.config | cut -d= -f2`
POSTGIS_MICRO_VERSION=`grep ^POSTGIS_MICRO_VERSION Version.config | cut -d= -f2`
echo $PATH

sh autogen.sh

if [ -f GNUMakefile ]; then
  make distclean
fi

#  --with-gdalconfig=${PROJECTS}/gdal/rel-${GDAL_VER}w${OS_BUILD}/bin/gdal-config
#  --without-raster

CPPFLAGS="-I${PGPATH}/include"  \
LDFLAGS="-L${PGPATH}/lib"  ./configure \
  --with-pgconfig=${PGPATH}/bin/pg_config \
  --with-geosconfig=${PROJECTS}/geos/rel-${GEOS_VER}w${OS_BUILD}/bin/geos-config \
  --without-raster
make clean
cd doc


mv postgis.xml postgis.xml.orig
sed -e "s:</title>:</title><subtitle><subscript>SVN Revision (<emphasis>${POSTGIS_SVN_REVISION}</emphasis>)</subscript></subtitle>:" postgis.xml.orig > postgis.xml

make pdf
rm -rf images
mkdir images
cp html/images/* images
make epub
make -e chunked-html 2>&1 | tee -a doc-errors.log
#make update-po
#make -C po/it_IT/ local-html
#make -C po/pt_BR/ local-html
#make pdf-localized

mv postgis.xml.orig postgis.xml
mkdir -p /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}
mkdir -p /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}/images
cp -R html/*.*  /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}
cp -R html/images/* /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}/images
chmod -R 755 /var/www/postgis_docs/manual-${POSTGIS_MAJOR_VERSION}.${POSTGIS_MINOR_VERSION}
cp -R *.pdf /var/www/postgis_stuff/
cp -R *.epub /var/www/postgis_stuff/