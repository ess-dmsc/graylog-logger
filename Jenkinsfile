// def failure_function(exception_obj, failureMessage) {
//     def toEmails = [[$class: 'DevelopersRecipientProvider']]
//     emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
//     // slackSend color: 'danger', message: "graylog-logger: " + failureMessage
//     throw exception_obj
// }

node('docker') {
    sh "docker ps --all"

    def image = docker.image('amues/centos-build-node:0.2.3')
    def name = "graylog-logger-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"
    def checkout_cmd = "git clone https://github.com/ess-dmsc/graylog-logger.git --branch ${env.BRANCH_NAME}"

    try {
        container = image.run("\
            --name ${name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy}"
        )

        stage('Checkout') {
            sh "docker exec ${name} sh -c \"${checkout_cmd}\""
            sh "docker exec ${name} ls"
        }
    } finally {
        container.stop()
    }

    sh "docker ps --all"
}

// node('docker') {
//     sh 'git config --get remote.origin.url'
//
//     docker.image('amues/centos-build-node:0.2.2').inside('--user jenkins') {
//         try {
//             stage("Checkout projects") {
//                 checkout scm
//             }
//         } catch (e) {
//             failure_function(e, 'Checkout failed')
//         }
//
//         try {
//             stage("Configure") {
//                 sh "rm -rf build"
//                 sh "mkdir build"
//                 sh "cd build && \
//                     HTTP_PROXY=$env.http_proxy \
//                     HTTPS_PROXY=$env.https_proxy \
//                     NO_PROXY=$env.no_proxy \
//                     conan install ../conan \
//                         -o build_everything=True \
//                         --build missing"
//                 sh "cd build && cmake .."
//             }
//         } catch (e) {
//             failure_function(e, 'Configure failed')
//         }
//
//         try {
//             stage("Build everything") {
//                 sh "cd build && make"
//             }
//         } catch (e) {
//             failure_function(e, 'Failed to compile')
//         }
//
//         try {
//             stage("Run cppcheck") {
//                 sh "cd build && make cppcheck"
//             }
//         } catch (e) {
//             failure_function(e, 'Cppcheck failed')
//         }
//
//         try {
//             stage("Run unit tests") {
//                 sh "./build/unit_tests/unit_tests --gtest_output=xml:AllResultsUnitTests.xml"
//                 // junit '*Tests.xml'
//             }
//         } catch (e) {
//             failure_function(e, 'Unit tests failed')
//         }
//
//         // try {
//         //     stage("Package") {
//         //         sh "rm -rf conan_packaging"
//         //         sh "HTTP_PROXY=$env.http_proxy \
//         //             HTTPS_PROXY=$env.https_proxy \
//         //             NO_PROXY=$env.no_proxy \
//         //             ./make_conan_package.sh ./conan"
//         //         sh "$DM_ROOT/virtualenv/conan/bin/conan upload \
//         //             --remote bintray-graylog-logger \
//         //             --confirm \
//         //             'graylog-logger/*'"
//         //     }
//         // } catch (e) {
//         //     failure_function(e, 'Packaging failed')
//         // }
//     }
// }

// node('clang-format') {
//     dir("code") {
//         try {
//             stage("Check formatting") {
//                 checkout scm
//                 sh "find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
//                     -exec $DM_ROOT/usr/bin/clangformatdiff.sh {} +"
//             }
//         } catch (e) {
//             failure_function(e, 'Formatting check failed')
//         }
//     }
//
//     if (currentBuild.previousBuild.result != "FAILURE") {
//         slackSend color: 'good', message: 'graylog-logger: Back in the green!'
//     }
// }
