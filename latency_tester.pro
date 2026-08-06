# Root project file - SUBDIRS configuration
# Builds the application and (in Debug) the test suite.

TEMPLATE = subdirs
SUBDIRS += LatencyTester

CONFIG(debug, debug|release) {
    SUBDIRS += Tests
    Tests.depends = LatencyTester
}

# Ensure LatencyTester builds first
LatencyTester.subdir = LatencyTester
Tests.subdir = LatencyTester/Tests
