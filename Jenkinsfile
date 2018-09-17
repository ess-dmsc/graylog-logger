@Library('ecdc-pipeline')
import ecdcpipeline.ContainerBuildNode
import ecdcpipeline.PipelineBuilder

project = "graylog-logger"

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

clangformat_os = "fedora25"
test_os = "ubuntu1804"

container_build_nodes = [
  'centos7': ContainerBuildNode.getDefaultContainerBuildNode('centos7'),
  'ubuntu1804': ContainerBuildNode.getDefaultContainerBuildNode('ubuntu1804'),
  'fedora25': new ContainerBuildNode('essdmscdm/fedora25-build-node:2.0.0', 'bash -e')
]

pipelineBuilder = new PipelineBuilder(this, containerBuildNodes)
pipelineBuilder.activateEmailFailureNotifications()
pipelineBuilder.activateSlackFailureNotifications()

builders = pipelineBuilder.createBuilders { container ->

  pipelineBuilder.stage("${container.key}: checkout") {
    dir(pipelineBuilder.project) {
      checkout scm
    }
    // Copy source code to container
    container.copyTo(pipelineBuilder.project, pipelineBuilder.project)
  }  // stage

  pipelineBuilder.stage("${container.key}: get dependencies") {
    container.sh """
      mkdir build
      cd build
      conan remote add --insert 0 ${conan_remote} ${local_conan_server}
      conan install --build outdated ../${pipelineBuilder.project}
    """
  }  // stage

  pipelineBuilder.stage("${container.key}: configure") {
    container.sh """
      cd build
      . ./activate_run.sh
      cmake ../${pipelineBuilder.project} -DBUILD_EVERYTHING=ON
    """
  }  // stage

  pipelineBuilder.stage("${container.key}: build") {
    container.sh """
      cd build
      . ./activate_run.sh
      make all
    """
  }  // stage

  if (container.key == test_os) {
    pipelineBuilder.stage("${container.key}: test") {
      def test_output = "TestResults.xml"
      container.sh """
        cd build
        . ./activate_run.sh
        ./unit_tests/unit_tests --gtest_output=xml:${test_output}
      """
      container.copyFrom('build', '.')
      junit "build/${test_output}"
    }  // stage
  }  // if

  if (container.key == clangformat_os) {
    pipelineBuilder.stage("${container.key}: check formatting") {
      container.sh """
        clang-format -version
        cd ${pipelineBuilder.project}
        find . \\\\( -name '*.cpp' -or -name '*.cxx' -or -name '*.h' -or -name '*.hpp' \\\\) \\
          -exec clangformatdiff.sh {} +
      """
    }  // stage

    pipelineBuilder.stage("${container.key}: cppcheck") {
      def test_output = "cppcheck.txt"
      container.sh """
        cd ${pipelineBuilder.project}
        cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" src/ 2> ${test_output}
      """
      container.copyFrom("${pipelineBuilder.project}/${test_output}", '.')
      step([
        $class: 'WarningsPublisher',
        parserConfigurations: [[
          parserName: 'Cppcheck Parser',
          pattern: 'cppcheck.txt'
        ]]
      ])
    }  // stage
  }  // if

}  // createBuilders

node('docker') {
  // Delete workspace when build is done
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

  builders['macOS'] = get_macos_pipeline()
  parallel builders
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    slackSend color: 'danger', message: "${project}: " + failureMessage
    throw exception_obj
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
                        sh "conan install --build=outdated ../code"
                        sh "source activate_run.sh && cmake ../code"
                    } catch (e) {
                        failure_function(e, 'MacOSX / CMake failed')
                    }

                    try {
                        sh "source activate_run.sh && make all"
                        sh "source activate_run.sh && ./unit_tests/unit_tests"
                    } catch (e) {
                        failure_function(e, 'MacOSX / build+test failed')
                    }
                }

            }
        }
    }
}
