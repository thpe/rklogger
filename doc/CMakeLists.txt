ADD_CUSTOM_TARGET(rklogger_man ALL)
ADD_CUSTOM_TARGET(man_dir ALL)
ADD_CUSTOM_COMMAND(TARGET man_dir COMMAND mkdir -p man)
ADD_CUSTOM_COMMAND(TARGET rklogger_man
   COMMAND gzip -c ${CMAKE_SOURCE_DIR}/doc/man/rklogger.1 > man/rklogger.1.gz
   DEPENDS man/rklogger.1)

#SET(manpages rklogger.1)
#FOREACH(manpage ${manpages})
#    ADD_CUSTOM_COMMAND(TARGET man
#       OUTPUT ${manpage}.gz
#       COMMAND gzip ${manpage}
#       SOURCE ${manpage}
#       DEPENDS ${manpage})
#ENDFOREACH(manpage)
