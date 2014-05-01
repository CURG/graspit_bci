#
# Qt qmake integration with Google Protocol Buffers compiler protoc
#
# To compile protocol buffers with qt qmake, specify PROTOS variable and
# include this file
#
# Example:
# LIBS += /usr/lib/libprotobuf.so
# PROTOS = a.proto b.proto
# include(protobuf.pri)
#
# By default protoc looks for .proto files (including the imported ones) in
# the current directory where protoc is run. If you need to include additional
# paths specify the PROTOPATH variable
#
PROTO_OUTPUT = $$PWD/gen_proto
VPATH += $$PROTOPATH $$PROTO_OUTPUT
INCLUDEPATH += $$PROTO_OUTPUT
LIBS += /usr/lib/libprotobuf.so

for(p, PROTOPATH):PROTOPATHS += --proto_path=$${p}
protobuf_decl.name = protobuf header
protobuf_decl.input = PROTOS
protobuf_decl.output = ${QMAKE_FILE_BASE}.pb.h
protobuf_decl.commands = $(MKDIR) $$PROTO_OUTPUT; protoc --cpp_out=$${PROTO_OUTPUT} $${PROTOPATHS} $$PWD/${QMAKE_FILE_NAME}
protobuf_decl.variable_out = GENERATED_FILES
QMAKE_EXTRA_COMPILERS += protobuf_decl

HEADERS += $$GENERATED_FILES

protobuf_impl.name = protobuf implementation
protobuf_impl.input = PROTOS
protobuf_impl.output = $${PROTO_OUTPUT}/${QMAKE_FILE_BASE}.pb.cc
protobuf_impl.depends = ${QMAKE_FILE_BASE}.pb.h
protobuf_impl.commands = $$escape_expand(\\n)
protobuf_impl.variable_out = GENERATED_SOURCES
QMAKE_EXTRA_COMPILERS += protobuf_impl

SOURCES += $$GENERATED_SOURCES
