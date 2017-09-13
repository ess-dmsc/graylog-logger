def project = "graylog-logger"

def centos = docker.image('essdmscdm/centos-build-node:0.7.0')
def fedora = docker.image('essdmscdm/fedora-build-node:0.2.0')

def base_container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

def run_in_container(container_name, script) {
    r = sh script: "docker exec ${container_name} sh -c \"${script}\"",
        returnStdout: true
    return r
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "@afonso.mukai ${project}-${env.BRANCH_NAME}: " + failureMessage
    throw exception_obj
}

node('docker') {
    try {
        def container_name = "${base_container_name}-centos"
        container = centos.run("\
            --name ${container_name} \
            --tty \
            --env http_proxy=${env.http_proxy} \
            --env https_proxy=${env.https_proxy} \
        ")

        stage('Checkout') {
            def checkout_script = """
                git clone https://github.com/ess-dmsc/${project}.git \
                    --branch ${env.BRANCH_NAME}
            """
            sh "docker exec ${container_name} sh -c \"${checkout_script}\""
        }

        stage('Get Dependencies') {
            def conan_remote = "ess-dmsc-local"
            def dependencies_script = """
                export http_proxy=''
                export https_proxy=''
                mkdir build
                cd build
                conan remote add \
                    --insert 0 \
                    ${conan_remote} ${local_conan_server}
                conan install ../${project} --build=missing
            """
            sh "docker exec ${container_name} sh -c \"${dependencies_script}\""
        }

        stage('Build') {
            def build_script = """
                cd build
                cmake3 ../${project} -DBUILD_EVERYTHING=ON
                make VERBOSE=1
            """
            sh "docker exec ${container_name} sh -c \"${build_script}\""
        }

        stage('Test') {
            def test_output = "AllResultsUnitTests.xml"
            def test_script = """
                ./build/unit_tests/unit_tests --gtest_output=xml:${test_output}
            """
            sh "docker exec ${container_name} sh -c \"${test_script}\""

            // Remove file outside container.
            sh "rm -f ${test_output}"
            // Copy results from container.
            sh "docker cp ${container_name}:/home/jenkins/${test_output} ."

            junit "${test_output}"
        }

        stage('Analyse') {
            def analysis_script = """
                make --directory=./build cppcheck
            """
            sh "docker exec ${container_name} sh -c \"${analysis_script}\""
        }

        stage('Archive') {
            def archive_script = """
                mkdir -p archive/${project}
                make -C build install DESTDIR=\\\$(pwd)/archive/${project}
                tar czvf ${project}.tar.gz -C archive ${project}
            """
            sh "docker exec ${container_name} sh -c \"${archive_script}\""

            // Remove file outside container.
            sh "rm -f ${project}.tar.gz"
            // Copy archive from container.
            sh "docker cp ${container_name}:/home/jenkins/${project}.tar.gz ."

            archiveArtifacts "${project}.tar.gz"
        }

        // Copy sources
        sh "docker cp ${container_name}:/home/jenkins/${project} ./srcs"
    } catch(e) {
        failure_function(e, 'Failed')
    } finally {
        container.stop()
    }

    // try {
    //     def container_name = "${base_container_name}-fedora"
    //     container = fedora.run("\
    //         --name ${container_name} \
    //         --tty \
    //         --env http_proxy=${env.http_proxy} \
    //         --env https_proxy=${env.https_proxy} \
    //     ")
    //
    //     sh "docker cp ./srcs ${container_name}:/home/jenkins/${project}"
    //     sh "rm -rf srcs"
    //
    //     run_in_container(container_name, """
    //         clang-format -version
    //     """)
    //
    //     stage('Check Formatting') {
    //         run_in_container(container_name, """
    //             cd ${project}
    //             find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
    //                 -exec clangformatdiff.sh {} +
    //         """)
    //     }
    //
    //     stage('Trigger Packaging') {
    //         pkg_commit = run_in_container(container_name, """
    //             cd ${project} && git rev-parse HEAD
    //         """)
    //
    //         build job: 'ess-dmsc/conan-graylog-logger/master',
    //             parameters: [
    //                 string(name: 'pkg_version', value: "master"),
    //                 string(name: 'pkg_commit', value: "${pkg_commit}"),
    //                 booleanParam(name: 'is_release', value: false)
    //             ],
    //             quietPeriod: 0,
    //             wait: false
    //     }
    // } catch(e) {
    //     failure_function(e, 'Failed')
    // } finally {
    //     container.stop()
    // }
}

try {
    if (currentBuild.previousBuild.result == "FAILURE") {
        slackSend color: 'good', message: "${project}-${env.BRANCH_NAME}: Back in the green!"
    }
} catch(e) { }
