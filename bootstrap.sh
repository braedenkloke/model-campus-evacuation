# !/bin/sh
#
# Bootstraps the project.

echo "Creating directory for external libraries..."
mkdir -v main/lib

mkdir -v tmp
cd tmp

echo "Installing dependencies..."
git clone https://github.com/d99kris/rapidcsv
cp -v rapidcsv/src/rapidcsv.h ../main/lib/
cd ..

rm -rf tmp

echo "Project bootstrapped."
