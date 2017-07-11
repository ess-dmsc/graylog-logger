def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "graylog-logger: " + failureMessage
    throw exception_obj
}

node('boost && centos7') {
    dir("code") {
        try {
            stage("Checkout projects") {
                checkout scm
            }
        } catch (e) {
            failure_function(e, 'Checkout failed')
        }
    }
    dir("build") {
        try {
            stage("Configure") {
                sh "rm -rf *"
                sh "PATH=/opt/dm_group/usr/bin:\$PATH \
                    $DM_ROOT/virtualenv/conan/bin/conan install \
                    ../code/conan \
                    -o build_everything=True \
                    --build missing"
                sh "PATH=$DM_ROOT/usr/bin:\$PATH cmake ../code"
            }
        } catch (e) {
            failure_function(e, 'Configure failed')
        }

        try {
            stage("Build everything") {
                sh "make"
            }
        } catch (e) {
            failure_function(e, 'Failed to compile')
        }

        try {
            stage("Run cppcheck") {
                sh "make cppcheck"
            }
        } catch (e) {
            failure_function(e, 'Cppcheck failed')
        }

        try {
            dir("unit_tests"){
                stage("Run unit tests") {
                    sh "./unit_tests --gtest_output=xml:AllResultsUnitTests.xml"
                    junit '*Tests.xml'
                }
            }
        } catch (e) {
            failure_function(e, 'Unit tests failed')
        }
    }
    dir("code") {
        try {
            stage("Package") {
                sh "PATH=/opt/dm_group/usr/bin:$DM_ROOT/virtualenv/conan/bin:\$PATH \
                    ./code/make_package.sh -k ./code/conan"
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
