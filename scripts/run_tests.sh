#/bin/sh

#cd onyx
#mkdir -p build_test && cd build_test

echo "Starting tests..."

run_tests()
{
  
  rm -Rf *
  echo "Building tests for $CC"
  cmake -DBUILD_TESTS=ON ..
  OUT=$?
  if [ $OUT -ne 0 ]; then
    echo "Configuring failed" && exit 1
  fi
  make onyx-test 
  OUT=$?
  if [ $OUT -ne 0 ]; then
    echo "Tests failed for $CC" && exit 1
  fi
  
  find . -name *.xml
  cat ./report.xml
}

export CC=gcc
export CXX=g++

#run_tests
