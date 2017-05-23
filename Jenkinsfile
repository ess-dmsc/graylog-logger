node('boost && centos7') {
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
            stage("Run cppcheck") {
                sh "make cppcheck"
            }
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: cppcheck failed'
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
}

node('clang-format') {
    dir("code") {
        try {
            stage("Checkout projects") {
                checkout scm
            }
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: Checkout failed'
            throw e
        }

        try {
            stage("Check formatting") {
                sh "find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec $DM_ROOT/usr/bin/clangformatdiff.sh {} +"
            }
        } catch (e) {
            slackSend color: 'danger', message: '@jonasn graylog-logger: Formatting check failed'
            throw e
        }
    }

    if (currentBuild.previousBuild.result == "FAILURE") {
        slackSend color: 'good', message: 'graylog-logger: Back in the green!'
    }
}
