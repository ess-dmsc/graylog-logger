def centos = docker.image('essdmscdm/centos-build-node:0.7.0')
def fedora = docker.image('essdmscdm/fedora-build-node:0.3.0')

def base_container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage +'\"\n\nCheck console output at $BUILD_URL to view the results.',
        recipientProviders: toEmails,
        subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger',
        message: "@afonso.mukai ${env.JOB_BASE_NAME}-${env.BRANCH_NAME}: " + failureMessage

    throw exception_obj
}

node('docker') {
    // Delete workspace when build is done
    cleanWs()

    dir("${env.JOB_BASE_NAME}") {
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
        sh "docker cp ${env.JOB_BASE_NAME} ${container_name}:/home/jenkins/${env.JOB_BASE_NAME}"

        stage('Get Dependencies') {
            def conan_remote = "ess-dmsc-local"
            sh """docker exec ${container_name} sh -c \"
                export http_proxy=''
                export https_proxy=''
                mkdir build
                cd build
                conan --version
                conan remote add \
                    --insert 0 \
                    ${conan_remote} ${local_conan_server}
                conan install ../${env.JOB_BASE_NAME} --build=missing
            \""""
        }

        stage('Build') {
            sh """docker exec ${container_name} sh -c \"
                cd build
                cmake --version
                cmake3 ../${env.JOB_BASE_NAME} -DBUILD_EVERYTHING=ON
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
                mkdir -p archive/${env.JOB_BASE_NAME}
                make -C build install DESTDIR=\\\$(pwd)/archive/${env.JOB_BASE_NAME}
                tar czvf ${env.JOB_BASE_NAME}.tar.gz -C archive ${env.JOB_BASE_NAME}
            \""""

            // Remove file outside container.
            sh "rm -f ${env.JOB_BASE_NAME}.tar.gz"
            // Copy archive from container.
            sh "docker cp ${container_name}:/home/jenkins/${env.JOB_BASE_NAME}.tar.gz ."

            archiveArtifacts "${env.JOB_BASE_NAME}.tar.gz"
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
        sh "docker cp ${env.JOB_BASE_NAME} ${container_name}:/home/jenkins/${env.JOB_BASE_NAME}"

        stage('Check Formatting') {
            sh """docker exec ${container_name} sh -c \"
                clang-format -version
                cd ${env.JOB_BASE_NAME}
                find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
                    -exec clangformatdiff.sh {} +
            \""""
        }

        // stage('Trigger Packaging') {
        //     def get_commit_script = """docker exec ${container_name} sh -c \"
        //         cd ${env.JOB_BASE_NAME}
        //         git rev-parse HEAD
        //     \""""
        //     pkg_commit = sh script: get_commit_script, returnStdout: true
        //     echo pkg_commit
        // }
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }
}

try {
    if (currentBuild.number > 1) {
        if (currentBuild.previousBuild.result == "FAILURE") {
            slackSend color: 'good',
                message: "${env.JOB_BASE_NAME}-${env.BRANCH_NAME}: Back in the green!"
        }
    }
} catch(e) { }
