node {
    stage("CMake") {
        cmake code/
    }
    
    stage("Build") {
        sh "make"
    }
    
    stage("Test") {
        sh "./unit_tests/LogTests --gtest_output=xml:LogTests.xml"
        junit "unit_tests/*Tests.xml"
    }
}