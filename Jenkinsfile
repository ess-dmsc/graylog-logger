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

container_build_nodes = [
  'ubuntu2204': new ContainerBuildNode('dockerregistry.esss.dk/ecdc_group/build-node-images/ubuntu22.04-build-node:4.0.0', 'bash -e')
]

pipeline_builder = new PipelineBuilder(this, container_build_nodes)
pipeline_builder.activateEmailFailureNotifications()

builders = pipeline_builder.createBuilders { container ->
  pipeline_builder.stage("${container.key}: checkout") {
    dir(pipeline_builder.project) {
      checkout scm
    }
    // Copy source code to container
    container.copyTo(pipeline_builder.project, pipeline_builder.project)
  }  // stage

  pipeline_builder.stage("${container.key}: check formatting") {
    if (!env.CHANGE_ID) {
      // Ignore non-PRs
      return
    }

    // Do clang-format of C++ files
    container.sh """
      ci/check-formatting
    """
  }  // stage

  pipeline_builder.stage("${container.key}: get dependencies") {
    container.sh """
      conan install --output-folder=build --build=outdated .
    """
  }  // stage

  pipeline_builder.stage("${container.key}: configure") {
    container.sh """
      cd build
      cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_EVERYTHING=ON
    """
  }  // stage

  pipeline_builder.stage("${container.key}: build") {
    container.sh """
      cd build
      make all unit_tests > ${container.key}-build.log
    """
    container.copyFrom("build/${container.key}-build.log", "${container.key}-build.log")
    archiveArtifacts "${container.key}-build.log"
  }  // stage

  pipeline_builder.stage("${container.key}: test") {
    def test_output = "TestResults.xml"
    container.sh """
      cd build
      ./unit_tests/unit_tests --gtest_output=xml:${test_output} --gtest_filter=-'GraylogConnectionCom.IPv6ConnectionTest'
    """
    container.copyFrom('build', '.')
    junit "build/${test_output}"
  }  // stage

  pipeline_builder.stage("${container.key}: cppcheck") {
    def test_output = "cppcheck.xml"
    container.sh """
      cd ${pipeline_builder.project}
      cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" --xml --xml-version=2 src/ 2> ${test_output}
    """
    container.copyFrom("${pipeline_builder.project}/${test_output}", '.')
    recordIssues(tools: [cppCheck(pattern: 'cppcheck.xml')])
  }  // stage
}  // createBuilders

node {
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

  try {
    parallel builders
    } catch (e) {
      pipeline_builder.handleFailureMessages()
      throw e
  }
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    throw exception_obj
}
