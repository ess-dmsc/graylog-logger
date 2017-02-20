node('boost') {
    dir("code") {
        try {
            stage("Checkout projects") {
                checkout scm
            } 
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: Checkout failed'
            throw e
        }
    }
    dir("build") {
        try {
            stage("Run CMake") {
                sh "cmake ../code"
            } 
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: CMake failed'
            throw e
        }
        
        try {
            stage("Build everything") {
                sh "make"
            }
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: Failed to compile'
            throw e
        }
        
        try {
            dir("unit_tests"){
                stage("Run unit tests") {
                    sh "./unit_tests --gtest_output=xml:AllResultsUnitTests.xml"
                    junit '*Tests.xml'
                }
            }
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: Unit tests failed'
            throw e
        }
    }
    if (null != currentBuild.previousBuild) {
        if (currentBuild.previousBuild.result == "FAILURE") {
            slackSend color: 'good', message: 'graylog-logger: Back in the green!'
        }
    }
}