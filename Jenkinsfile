def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "graylog-logger: " + failureMessage
    throw exception_obj
}

node('docker') {
    docker.image('amues/centos-build-node:0.2.1').inside {
        environment {
            http_proxy = "$env.http_proxy"
            https_proxy = '$env.https_proxy'
            no_proxy = '$env.no_proxy'
        }

        try {
            stage("Checkout projects") {
                sh "env"
                checkout scm
            }
        } catch (e) {
            failure_function(e, 'Checkout failed')
        }

        try {
            stage("Configure") {
                sh "rm -rf build"
                sh "mkdir build"
                sh "cd build && \
                    conan install ../conan \
                    -o build_everything=True \
                    --build missing"
                sh "cd build && cmake .."
            }
        } catch (e) {
            failure_function(e, 'Configure failed')
        }

        try {
            stage("Build everything") {
                sh "cd build && make"
            }
        } catch (e) {
            failure_function(e, 'Failed to compile')
        }

        try {
            stage("Run cppcheck") {
                sh "cd build && make cppcheck"
            }
        } catch (e) {
            failure_function(e, 'Cppcheck failed')
        }

        try {
            stage("Run unit tests") {
                sh "./build/unit_tests/unit_tests --gtest_output=xml:AllResultsUnitTests.xml"
                junit '*Tests.xml'
            }
        } catch (e) {
            failure_function(e, 'Unit tests failed')
        }
        try {
            stage("Package") {
                sh "./make_conan_package.sh ./conan"
                // sh "$DM_ROOT/virtualenv/conan/bin/conan upload \
                //     --remote bintray-graylog-logger \
                //     --confirm \
                //     'graylog-logger/*'"
            }
        } catch (e) {
            failure_function(e, 'Packaging failed')
        }
    }
}

node('clang-format') {
    dir("code") {
        try {
            stage("Check formatting") {
                checkout scm
                sh "find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec $DM_ROOT/usr/bin/clangformatdiff.sh {} +"
            }
        } catch (e) {
            failure_function(e, 'Formatting check failed')
        }
    }

    if (currentBuild.previousBuild.result != "FAILURE") {
        slackSend color: 'good', message: 'graylog-logger: Back in the green!'
    }
}
