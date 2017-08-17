def project = "graylog-logger"

def checkout_script = """
    git clone https://github.com/ess-dmsc/${project}.git \
        --branch ${env.BRANCH_NAME}
"""

def configure_script = """
    mkdir build
    cd build
    conan install ../${project}/conan \
        -o build_everything=True \
        --build=missing
    cmake3 ../${project} -DBUILD_EVERYTHING=ON
"""

def build_script = "make --directory=./build"

def test_output = "AllResultsUnitTests.xml"
def test_script = """
    ./build/unit_tests/unit_tests --gtest_output=xml:${test_output}
"""

def cppcheck_script = "make --directory=./build cppcheck"

def package_script = """
    cd ${project}
    ./make_conan_package.sh ./conan
"""

def formatting_script = """
    cd ${project}
    find . \\( -name '*.cpp' -or -name '*.h' -or -name '*.hpp' \\) \
        -exec clangformatdiff.sh {} +
"""

def centos = docker.image('essdmscdm/centos-build-node:0.2.5')
def fedora = docker.image('essdmscdm/fedora-build-node:0.1.2')

def container_name = "${project}-${env.BRANCH_NAME}-${env.BUILD_NUMBER}"

node('docker') {
    def run_args = "\
        --name ${container_name} \
        --tty \
        --env http_proxy=${env.http_proxy} \
        --env https_proxy=${env.https_proxy}"

    try {
        container = centos.run(run_args)

        stage('Checkout') {
            sh "docker exec ${container_name} sh -c \"${checkout_script}\""
        }

        stage('Configure') {
            sh "docker exec ${container_name} sh -c \"${configure_script}\""
        }

        stage('Build') {
            sh "docker exec ${container_name} sh -c \"${build_script}\""
        }

        stage('Tests') {
            sh "docker exec ${container_name} sh -c \"${test_script}\""
            sh "rm -f ${test_output}" // Remove file outside container.
            sh "docker cp ${container_name}:/home/jenkins/${test_output} ."
            junit "${test_output}"
        }

        stage('Cppcheck') {
            sh "docker exec ${container_name} sh -c \"${cppcheck_script}\""
        }

        stage('Package') {
            sh "docker exec ${container_name} sh -c \"${package_script}\""
        }

        sh "docker cp ${container_name}:/home/jenkins/${project} ./srcs"
    } finally {
        container.stop()
    }

    try {
        container = fedora.run(run_args)

        sh "docker cp ./srcs ${container_name}:/home/jenkins/${project}"
        sh "rm -rf srcs"

        stage('Formatting') {
            sh "docker exec ${container_name} sh -c \"${formatting_script}\""
        }
    } finally {
        container.stop()
    }
}
