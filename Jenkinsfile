project = "graylog-logger"

def centos = docker.image('essdmscdm/centos7-build-node:1.0.1')
def fedora = docker.image('essdmscdm/fedora-build-node:0.3.0')

def base_container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage +'\"\n\nCheck console output at $BUILD_URL to view the results.',
        recipientProviders: toEmails,
        subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger',
        message: "@afonso.mukai ${project}-${env.BRANCH_NAME}: " + failureMessage

    throw exception_obj
}

node('docker') {
    // Delete workspace when build is done
    cleanWs()

    dir("${project}") {
        stage('Checkout') {
            scm_vars = checkout scm
        }
    }

    try {
        def container_name = "${base_container_name}-centos"
        container = centos.run("\
            --name ${container_name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy} \
        ")

        // Copy sources to container.
        sh "docker cp ${project} ${container_name}:/home/jenkins/${project}"

        stage('Get Dependencies') {
            def conan_remote = "ess-dmsc-local"
            sh """docker exec ${container_name} sh -c \"
                mkdir build
                cd build
                conan --version
                conan remote add \
                    --insert 0 \
                    ${conan_remote} ${local_conan_server}
                conan install ../${project} --build=missing
            \""""
        }

        stage('Build') {
            sh """docker exec ${container_name} sh -c \"
                cd build
                cmake --version
                cmake3 ../${project} -DBUILD_EVERYTHING=ON
                make --version
                make VERBOSE=1
            \""""
        }

        stage('Test') {
            def test_output = "AllResultsUnitTests.xml"
            sh """docker exec ${container_name} sh -c \"
                ./build/unit_tests/unit_tests --gtest_output=xml:${test_output}
            \""""

            // Remove file outside container.
            sh "rm -f ${test_output}"
            // Copy results from container.
            sh "docker cp ${container_name}:/home/jenkins/${test_output} ."

            junit "${test_output}"
        }

        stage('Analyse') {
            sh """docker exec ${container_name} sh -c \"
                cppcheck --version
                make --directory=./build cppcheck
            \""""
        }

        stage('Archive') {
            sh """docker exec ${container_name} sh -c \"
                mkdir -p archive/${project}
                make -C build install DESTDIR=\\\$(pwd)/archive/${project}
                tar czvf ${project}.tar.gz -C archive ${project}
            \""""

            // Remove file outside container.
            sh "rm -f ${project}.tar.gz"
            // Copy archive from container.
            sh "docker cp ${container_name}:/home/jenkins/${project}.tar.gz ."

            // Create file with commit information.
            sh "echo ${scm_vars.GIT_COMMIT} > GIT_COMMIT"

            archiveArtifacts "${project}.tar.gz,GIT_COMMIT"
        }
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }

    try {
        def container_name = "${base_container_name}-fedora"
        container = fedora.run("\
            --name ${container_name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy} \
        ")

        // Copy sources to container.
        sh "docker cp ${project} ${container_name}:/home/jenkins/${project}"

        stage('Check Formatting') {
            formattingResult = sh script: """docker exec ${container_name} sh -c \"
                clang-format -version
                cd ${project}
                find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec clangformatdiff.sh {} +
            \"""", returnStatus: true
            if (formattingResult != 0) {
              currentBuild.result = 'UNSTABLE'
            }
        }
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }

    stage('Trigger Packaging') {
        build job: "ess-dmsc/conan-graylog-logger/${env.BRANCH_NAME}",
            wait: false
    }
}

try {
    if (currentBuild.number > 1) {
        if (currentBuild.previousBuild.result == "FAILURE") {
            slackSend color: 'good',
                message: "${project}-${env.BRANCH_NAME}: Back in the green!"
        }
    }
} catch(e) { }
