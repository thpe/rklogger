ADD_CUSTOM_TARGET(man ALL)
ADD_CUSTOM_TARGET(rklogger.1.gz ALL)
ADD_CUSTOM_COMMAND(TARGET rklogger.1.gz
   OUTPUT rklogger.1.gz
   COMMAND gzip -c ${CMAKE_SOURCE_DIR}/doc/man/rklogger.1 > rklogger.1.gz
   SOURCE  rklogger.1.gz
   DEPENDS rklogger.1)
#SET(manpages rklogger.1)
#FOREACH(manpage ${manpages})
#    ADD_CUSTOM_COMMAND(TARGET man
#       OUTPUT ${manpage}.gz
#       COMMAND gzip ${manpage}
#       SOURCE ${manpage}
#       DEPENDS ${manpage})
#ENDFOREACH(manpage)
