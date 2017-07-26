node('docker') {
    sh "docker ps --all"

    def centos = docker.image('amues/centos-build-node:0.2.5')
    def fedora = docker.image('amues/fedora-build-node:0.1.2')

    def name = "graylog-logger-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

    try {
        container = centos.run("\
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
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Build') {
            cmd = "make --directory=./build"
            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Tests') {
            cmd = "./build/unit_tests/unit_tests --gtest_output=xml:AllResultsUnitTests.xml"
            sh "docker exec ${name} sh -c \"${cmd}\""
            sh "rm -f AllResultsUnitTests.xml" // Remove file outside container.
            sh "docker cp ${name}:/home/jenkins/AllResultsUnitTests.xml ."
            junit "AllResultsUnitTests.xml"
        }

        stage('Cppcheck') {
            cmd = "make --directory=./build cppcheck"
            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Formatting') {
            cmd = "make --directory=./build cppcheck"
            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        stage('Package') {
            cmd = """
                cd graylog-logger
                ./make_conan_package.sh ./conan
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }

        sh "docker cp ${name}:/home/jenkins/graylog-logger ."
    } finally {
        container.stop()
    }

    try {
        container = fedora.run("\
            --name ${name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy}"
        )

        sh "docker cp graylog-logger ${name}:/home/jenkins"
        sh "rm -rf graylog-logger"

        stage('Formatting') {
            cmd = """
                cd graylog-logger
                find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec clangformatdiff.sh {} +
            """

            sh "docker exec ${name} sh -c \"${cmd}\""
        }
    } finally {
        container.stop()
    }

    sh "docker ps --all"
}
