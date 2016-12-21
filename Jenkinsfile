node {
    stage("CMake") {
        cmake code/
    }
    
    stage("Build") {
        sh "make"
    }
    
    stage("Test") {
        sh "./tests/LogTests --gtest_output=xml:LogTests.xml"
        junit "tests/*Tests.xml"
    }
}