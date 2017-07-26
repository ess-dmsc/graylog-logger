node('docker') {
    sh "docker ps --all"

    def image = docker.image('amues/centos-build-node:0.2.3')
    def name = "graylog-logger-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

    try {
        container = image.run("\
            --name ${name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy}"
        )

        stage('Checkout') {
            cmd = """
                git clone https://github.com/ess-dmsc/graylog-logger.git \
                    --branch ${env.BRANCH_NAME}
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Configure') {
            cmd = """
                mkdir build
                cd build
                conan install ../graylog-logger/conan \
                    -o build_everything=True \
                    --build=missing
                cmake3 ../graylog-logger -DBUILD_EVERYTHING=ON
                pwd
                ls
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Build') {
            cmd = """
                make --directory=./build
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Tests') {
            cmd = """
                ./build/unit_tests/unit_tests --gtest_output=xml:AllResultsUnitTests.xml
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
            sh "docker cp ${name}:/home/jenkins/AllResultsUnitTests.xml ."
            junit "AllResultsUnitTests.xml"
        }

        stage('Cppcheck') {
            cmd = """
                make --directory=./build cppcheck
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Package') {
            cmd = """
                ./graylog-logger/make_conan_package.sh
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }
    } finally {
        container.stop()
    }

    sh "docker ps --all"
}

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
