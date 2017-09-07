def project = "graylog-logger"

def centos = docker.image('essdmscdm/centos-build-node:0.3.0')
def fedora = docker.image('essdmscdm/fedora-build-node:0.1.3')

def run_in_container(container_name, script) {
    sh "docker exec ${container_name} sh -c \"${script}\""
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "@afonso.mukai ${project}-${env.BRANCH_NAME}: " + failureMessage
    throw exception_obj
}

node('docker') {
    try {
        def container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"
        def run_args = "\
            --name ${container_name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy}"

        container = centos.run(run_args)

        stage('Checkout') {
            run_in_container(container_name, """
                git clone https://github.com/ess-dmsc/${project}.git \
                    --branch ${env.BRANCH_NAME}
            """)
        }

        stage('Configure') {
            run_in_container(container_name, """
                mkdir build
                cd build
                conan install ../${project}/conan \
                    -o build_everything=True \
                    --build=missing
                cmake3 ../${project} -DBUILD_EVERYTHING=ON
            """)
        }

        stage('Build') {
            run_in_container(container_name, "make --directory=./build")
        }

        stage('Tests') {
            def test_output = "AllResultsUnitTests.xml"
            run_in_container(container_name, """
                ./build/unit_tests/unit_tests --gtest_output=xml:${test_output}
            """)

            // Copy results from container.
            sh "rm -f ${test_output}" // Remove file outside container.
            sh "docker cp ${container_name}:/home/jenkins/${test_output} ."

            junit "${test_output}"
        }

        stage('Cppcheck') {
            run_in_container(container_name, """
                make --directory=./build cppcheck
            """)
        }

        stage('Archive') {
            run_in_container(container_name, """
                mkdir -p archive/graylog-logger
                make -C build install DESTDIR=\$(pwd)/../archive/graylog-logger
                tar czvf graylog-logger.tar.gz -C archive graylog-logger
            """)

            // Copy archive from container.
            sh "rm -f graylog-logger.tar.gz" // Remove file outside container.
            sh "docker cp ${container_name}:/home/jenkins/graylog-logger.tar.gz ."

            archiveArtifacts 'graylog-logger.tar.gz'
        }

        // Copy sources
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

if (currentBuild.previousBuild.result != "FAILURE") {
    slackSend color: 'good', message: "${project}-${env.BRANCH_NAME}: Back in the green!"
}
