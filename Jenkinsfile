project = "graylog-logger"
clangformat_os = "fedora25"
test_os = "centos7"

images = [
        'centos7': [
                'name': 'essdmscdm/centos7-build-node:3.1.0',
                'sh'  : '/usr/bin/scl enable rh-python35 devtoolset-6 -- /bin/bash -e'
        ],
        'fedora25'    : [
                'name': 'essdmscdm/fedora25-build-node:2.0.0',
                'sh'  : 'bash -e'
        ]/*,*/
/*        'ubuntu1804'  : [
                'name': 'essdmscdm/ubuntu18.04-build-node:1.1.0',
                'sh'  : 'bash -e'
        ]*/
]

base_container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

def Object container_name(image_key) {
    return "${base_container_name}-${image_key}"
}

// Set number of old builds to keep.
properties([[
    $class: 'BuildDiscarderProperty',
    strategy: [
        $class: 'LogRotator',
        artifactDaysToKeepStr: '',
        artifactNumToKeepStr: '10',
        daysToKeepStr: '',
        numToKeepStr: ''
    ]
]]);

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "${project}: " + failureMessage
    throw exception_obj
}

def create_container(image_key) {
    def image = docker.image(images[image_key]['name'])
    def container = image.run("\
        --name ${container_name(image_key)} \
        --tty \
        --network=host \
        --env http_proxy=${env.http_proxy} \
        --env https_proxy=${env.https_proxy} \
        --env local_conan_server=${env.local_conan_server} \
          ")
}

def docker_copy_code(image_key) {
    def custom_sh = images[image_key]['sh']
    sh "docker cp ${project} ${container_name(image_key)}:/home/jenkins/${project}"
    sh """docker exec --user root ${container_name(image_key)} ${custom_sh} -c \"
                        chown -R jenkins.jenkins /home/jenkins/${project}
                        \""""
}

def docker_dependencies(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def conan_remote = "ess-dmsc-local"
        def dependencies_script = """
                        mkdir build
                        cd build
                        conan remote add \
                            --insert 0 \
                            ${conan_remote} ${local_conan_server}
                        conan install --build=outdated ../${project}/
                    """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${dependencies_script}\""
    } catch (e) {
        failure_function(e, "Add conan remote for (${container_name(image_key)}) failed")
    }
}

def docker_cmake(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def configure_script = """
                        cd build
                        . ./activate_run.sh
                        cmake ../${project} -DBUILD_EVERYTHING=ON
                    """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${configure_script}\""
    } catch (e) {
        failure_function(e, "CMake step for (${container_name(image_key)}) failed")
    }
}

def docker_build(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def build_script = """
                      cd build
                      . ./activate_run.sh
                      make all
                  """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${build_script}\""
    } catch (e) {
        failure_function(e, "Build step for (${container_name(image_key)}) failed")
    }
}

def docker_test(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def test_output = "TestResults.xml"
        def test_script = """
                        cd build
                        . ./activate_run.sh
                        ./unit_tests/unit_tests -- --gtest_output=xml:${test_output}
                    """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${test_script}\""
        sh "docker cp ${container_name(image_key)}:/home/jenkins/build ./"
        junit "build/${test_output}"
    } catch (e) {
        failure_function(e, "Unit testing for (${container_name(image_key)}) failed")
    }
}

def docker_formatting(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def script = """
                    clang-format -version
                    cd ${project}
                    find . \\\\( -name '*.cpp' -or -name '*.cxx' -or -name '*.h' -or -name '*.hpp' \\\\) \\
                        -exec clangformatdiff.sh {} +
                  """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${script}\""
    } catch (e) {
        failure_function(e, "Check formatting step for (${container_name(image_key)}) failed")
    }
}

def docker_cppcheck(image_key) {
    try {
        def custom_sh = images[image_key]['sh']
        def test_output = "cppcheck.txt"
        def cppcheck_script = """
                        cd ${project}
                        cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" src/ 2> ${test_output}
                    """
        sh "docker exec ${container_name(image_key)} ${custom_sh} -c \"${cppcheck_script}\""
        sh "docker cp ${container_name(image_key)}:/home/jenkins/${project}/${test_output} ."
    } catch (e) {
        failure_function(e, "Cppcheck step for (${container_name(image_key)}) failed")
    }
}

def get_pipeline(image_key) {
    return {
        stage("${image_key}") {

            try {
                create_container(image_key)

                docker_copy_code(image_key)
                docker_dependencies(image_key)

                docker_cmake(image_key)
                    
                docker_build(image_key)

                if (image_key == test_os) {
                    docker_test(image_key)
                }

                if (image_key == clangformat_os) {
                    docker_formatting(image_key)
                    docker_cppcheck(image_key)
                    step([$class: 'WarningsPublisher', parserConfigurations: [[parserName: 'Cppcheck Parser', pattern: 'cppcheck.txt']]])
                }

            } catch (e) {
                failure_function(e, "Unknown build failure for ${image_key}")
            } finally {
                sh "docker stop ${container_name(image_key)}"
                sh "docker rm -f ${container_name(image_key)}"
            }
        }
    }
}

def get_macos_pipeline()
{
    return {
        stage("macOS") {
            node ("macos") {
                // Delete workspace when build is done
                cleanWs()

                dir("${project}/code") {
                    try {
                        checkout scm
                    } catch (e) {
                        failure_function(e, 'MacOSX / Checkout failed')
                    }
                }

                dir("${project}/build") {
                    try {
                        sh "cmake ../code"
                    } catch (e) {
                        failure_function(e, 'MacOSX / CMake failed')
                    }

                    try {
                        sh "make all"
                        sh ". ./activate_run.sh && ./unit_tests/unit_tests"
                    } catch (e) {
                        failure_function(e, 'MacOSX / build+test failed')
                    }
                }

            }
        }
    }
}

node('docker') {
    cleanWs()

    stage('Checkout') {
        dir("${project}") {
            try {
                scm_vars = checkout scm
            } catch (e) {
                failure_function(e, 'Checkout failed')
            }
        }
    }

    def builders = [:]
    for (x in images.keySet()) {
        def image_key = x
        builders[image_key] = get_pipeline(image_key)
    }
    builders['macOS'] = get_macos_pipeline()

    parallel builders

    // Delete workspace when build is done
    cleanWs()
}