# Top level NDK Makefile for Android application
# Module settings for CUGL
SUPPORT_OPENGL := true
SUPPORT_AUDIO  := true
SUPPORT_SCENE2 := true
SUPPORT_SCENE3 := false
SUPPORT_PHYSICS2 := false
SUPPORT_NETCODE  := false
SUPPORT_PHYSICS2_DISTRIB := __CUGL_DISTRIB_PHYSICS2__
SUPPORT_PHYSICS2_LIGHTS  := __CUGL_DISTRIB_LIGHTS__

# We just invoke the subdirectories
include $(call all-subdir-makefiles)
