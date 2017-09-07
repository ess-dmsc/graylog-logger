def project = "graylog-logger"
def centos = docker.image('essdmscdm/centos-build-node:0.2.5')
def fedora = docker.image('essdmscdm/fedora-build-node:0.1.2')

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "@jonasn graylog-logger: " + failureMessage
    throw exception_obj
}

node('docker') {
    def container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"
    def run_args = "\
        --name ${container_name} \
        --tty \
        --env http_proxy=${env.http_proxy} \
        --env https_proxy=${env.https_proxy}"

    try {
        container = centos.run(run_args)

        stage('Checkout') {
            def checkout_script = """
                git clone https://github.com/ess-dmsc/${project}.git \
                    --branch ${env.BRANCH_NAME}
            """
            sh "docker exec ${container_name} sh -c \"${checkout_script}\""
        }

        stage('Configure') {
            def configure_script = """
                mkdir build
                cd build
                conan install ../${project}/conan \
                    -o build_everything=True \
                    --build=missing
                cmake3 ../${project} -DBUILD_EVERYTHING=ON
            """
            sh "docker exec ${container_name} sh -c \"${configure_script}\""
        }

        stage('Build') {
            def build_script = "make --directory=./build"
            sh "docker exec ${container_name} sh -c \"${build_script}\""
        }

        stage('Tests') {
            def test_output = "AllResultsUnitTests.xml"
            def test_script = """
                ./build/unit_tests/unit_tests --gtest_output=xml:${test_output}
            """
            sh "docker exec ${container_name} sh -c \"${test_script}\""
            sh "rm -f ${test_output}" // Remove file outside container.
            sh "docker cp ${container_name}:/home/jenkins/${test_output} ."
            junit "${test_output}"
        }

        stage('Cppcheck') {
            def cppcheck_script = "make --directory=./build cppcheck"
            sh "docker exec ${container_name} sh -c \"${cppcheck_script}\""
        }

        // stage('Package') {
        //     def package_script = """
        //         cd ${project}
        //         ./make_conan_package.sh ./conan
        //     """
        //     sh "docker exec ${container_name} sh -c \"${package_script}\""
        // }

        stage('Archive') {
            def package_script = """
                mkdir -p archive/graylog-logger
                make -C build install DESTDIR=\$(pwd)/../archive/graylog-logger
                tar czvf graylog-logger.tar.gz -C archive graylog-logger
            """
            sh "docker exec ${container_name} sh -c \"${package_script}\""
            // Copy archive from container.
            sh "rm -f graylog-logger.tar.gz" // Remove file outside container.
            sh "docker cp ${container_name}:/home/jenkins/graylog-logger.tar.gz ."
            archiveArtifacts 'graylog-logger.tar.gz'
        }

        sh "docker cp ${container_name}:/home/jenkins/${project} ./srcs"
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }

    try {
        container = fedora.run(run_args)

        sh "docker cp ./srcs ${container_name}:/home/jenkins/${project}"
        sh "rm -rf srcs"

        stage('Formatting') {
            def formatting_script = """
                cd ${project}
                find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec clangformatdiff.sh {} +
            """
            sh "docker exec ${container_name} sh -c \"${formatting_script}\""
        }
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }
}
