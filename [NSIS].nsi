; NSIS script NSIS-2.25
; Install

SetCompressor /SOLID lzma
SetCompressorDictSize 8

; --------------------
; HEADER SIZE: 18623
; START HEADER SIZE: 288
; MAX STRING LENGTH: 1024
; STRING CHARS: 5185

OutFile [NSIS].exe
!include WinMessages.nsh

InstallDirRegKey HKCU "Software\Bomberman ECE" ""
LicenseBkColor /windows


; --------------------
; LANG TABLES: 1
; LANG STRINGS: 84

Name "Bomberman ECE"
BrandingText "Nullsoft Install System v2.16"

; LANG: 1036
LangString LSTR_0 1036 "Nullsoft Install System v2.16"
LangString LSTR_1 1036 "Installation de $(LSTR_2)"
LangString LSTR_2 1036 "Bomberman ECE"
LangString LSTR_3 1036 "Espace disponible : "
LangString LSTR_4 1036 "Espace requis : "
LangString LSTR_5 1036 "Impossible d'écrire : "
LangString LSTR_10 1036 "Création du dossier : "
LangString LSTR_11 1036 "Création du raccourci : "
LangString LSTR_12 1036 "Création de la désinstallation : "
LangString LSTR_13 1036 "Suppression : "
LangString LSTR_14 1036 "Suppression au redémarrage : "
LangString LSTR_15 1036 "Erreur lors de la création du raccourci : "
LangString LSTR_16 1036 "Erreur de la création : "
LangString LSTR_17 1036 "Erreur lors de la décompression des données ! Installation corrompue ?"
LangString LSTR_21 1036 "Extraction : "
LangString LSTR_22 1036 "Extraction : erreur d'écriture du fichier "
LangString LSTR_25 1036 "Destination : "
LangString LSTR_26 1036 "Suppression du dossier : "
LangString LSTR_29 1036 "Passé : "
LangString LSTR_30 1036 "Copier les Détails dans le Presse-papier"
LangString LSTR_32 1036 o
LangString LSTR_33 1036 K
LangString LSTR_34 1036 M
LangString LSTR_35 1036 G
LangString LSTR_36 1036 "Si vous acceptez les termes de la licence utilisateur, cliquez sur J'accepte pour continuer. Vous devez accepter la licence utilisateur pour installer $(LSTR_83)."
LangString LSTR_37 1036 "Licence utilisateur"
LangString LSTR_38 1036 "Veuillez examiner les termes de la licence avant d'installer $(LSTR_83)."
LangString LSTR_39 1036 "Appuyez sur Page Suivante pour lire le reste de la licence utilisateur."
LangString LSTR_40 1036 "Choisissez les composants"
LangString LSTR_41 1036 "Choisissez les composants de $(LSTR_83) que vous souhaitez installer."
LangString LSTR_42 1036 Description
LangString LSTR_43 1036 "Passer le curseur de votre souris sur un composant pour voir sa description."
LangString LSTR_44 1036 "Choisissez le dossier d'installation"
LangString LSTR_45 1036 "Choisissez le dossier dans lequel installer $(LSTR_83)."
LangString LSTR_46 1036 "Choisissez un dossier dans le menu Démarrer"
LangString LSTR_47 1036 "Choisissez un dossier dans le menu Démarrer pour les raccourcis de l'application."
LangString LSTR_48 1036 0
LangString LSTR_49 1036 "Erreur lors de l'ouverture du fichier en écriture : $\r$\n$\t$\"$0$\"$\r$\nAppuyez sur Abandonner pour annuler l'installation,$\r$\nRéessayer pour réessayer l'écriture du fichier, ou$\r$\nIgnorer pour passer ce fichier"
LangString LSTR_50 1036 "Ne pas créer de raccourcis"
LangString LSTR_51 1036 "Choisissez le dossier du menu Démarrer dans lequel vous voulez placer les raccourcis du programme. Vous pouvez également entrer un nouveau nom pour créer un nouveau dossier."
LangString LSTR_52 1036 "Installation en cours"
LangString LSTR_53 1036 "Veuillez patienter pendant que $(LSTR_83) est en train d'être installé."
LangString LSTR_54 1036 "Installation terminée"
LangString LSTR_55 1036 "L'installation s'est terminée avec succès."
LangString LSTR_56 1036 "Installation interrompue"
LangString LSTR_57 1036 "L'installation n'a pas été terminée."
LangString LSTR_58 1036 "MS Shell Dlg"
LangString LSTR_59 1036 8
LangString LSTR_60 1036 "Êtes-vous sûr de vouloir quitter l'installation de $(LSTR_2) ?"
LangString LSTR_61 1036 "Le bomberman par défaut. Si vous le déselectionnez, vous n'installerez rien."
LangString LSTR_62 1036 "Les sources de bomberman, vous avez besoin de Bomberman ECE pour pouvoir les utiliser."
LangString LSTR_63 1036 Personnalisée
LangString LSTR_64 1036 Annuler
LangString LSTR_65 1036 "< &Précédent"
LangString LSTR_66 1036 J'a&ccepte
LangString LSTR_67 1036 "Cliquez sur Suivant pour continuer."
LangString LSTR_68 1036 "Cochez les composants que vous désirez installer et décochez ceux que vous ne désirez pas installer. $_CLICK"
LangString LSTR_69 1036 "Sélectionnez le type d'installation :"
LangString LSTR_70 1036 "Ou, sélectionnez les composants optionnels que vous voulez installer :"
LangString LSTR_71 1036 "Sélectionnez les composants à installer :"
LangString LSTR_72 1036 "&Suivant >"
LangString LSTR_73 1036 "Ceci installera $(LSTR_83) dans le dossier suivant. Pour installer dans un autre dossier, cliquez sur Parcourir et choisissez un autre dossier. $_CLICK"
LangString LSTR_74 1036 "Dossier d'installation"
LangString LSTR_75 1036 P&arcourir...
LangString LSTR_76 1036 "Sélectionnez le dossier d'installation pour $(LSTR_83) :"
LangString LSTR_77 1036 &Installer
LangString LSTR_78 1036 "Cliquez sur Installer pour démarrer l'installation."
LangString LSTR_79 1036 "P&lus d'infos"
LangString LSTR_80 1036 Terminé
LangString LSTR_81 1036 " "
LangString LSTR_82 1036 &Fermer
LangString LSTR_83 1036 "Bomberman ECE"


; --------------------
; VARIABLES: 5

Var _0_
Var _1_
Var _2_
Var _3_
Var _4_


InstType $(LSTR_63)    ;  Personnalisée
InstallDir "$PROGRAM_FILES\Bomberman ECE"
; install_directory_auto_append = "Bomberman ECE"


; --------------------
; PAGES: 6

; Page 0
Page license func_0 func_5 func_9 /ENABLECANCEL
  LicenseText $(LSTR_36) $(LSTR_66)    ;  "Si vous acceptez les termes de la licence utilisateur, cliquez sur J'accepte pour continuer. Vous devez accepter la licence utilisateur pour installer $(LSTR_83)." J'a&ccepte "Bomberman ECE"
  LicenseData [LICENSE].txt

; Page 1
Page components func_10 func_15 func_26 /ENABLECANCEL
  ComponentsText $(LSTR_68) $(LSTR_69) $(LSTR_70)    ;  "Cochez les composants que vous désirez installer et décochez ceux que vous ne désirez pas installer. $_CLICK" "Sélectionnez le type d'installation :" "Ou, sélectionnez les composants optionnels que vous voulez installer :"

; Page 2
Page directory func_27 func_32 func_33 /ENABLECANCEL
  DirText $(LSTR_73) $(LSTR_74) $(LSTR_75) $(LSTR_76)    ;  "Ceci installera $(LSTR_83) dans le dossier suivant. Pour installer dans un autre dossier, cliquez sur Parcourir et choisissez un autre dossier. $_CLICK" "Dossier d'installation" P&arcourir... "Sélectionnez le dossier d'installation pour $(LSTR_83) :" "Bomberman ECE" "Bomberman ECE"
  DirVar $CMDLINE

; Page 3
Page custom func_34 func_80 /ENABLECANCEL

; Page 4
Page instfiles func_81 func_86 func_87
  CompletedText $(LSTR_80)    ;  Terminé
  DetailsButtonText $(LSTR_79)    ;  "P&lus d'infos"

/*
; Page 5
Page COMPLETED
*/


; --------------------
; SECTIONS: 2
; COMMANDS: 367

Function func_0    ; Page 0, Pre
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_37)    ;  "Licence utilisateur"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_38)    ;  "Veuillez examiner les termes de la licence avant d'installer $(LSTR_83)." "Bomberman ECE"
FunctionEnd


Function func_5    ; Page 0, Show
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1040
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_39)    ;  "Appuyez sur Page Suivante pour lire le reste de la licence utilisateur."
FunctionEnd


Function func_9    ; Page 0, Leave
FunctionEnd


Function func_10    ; Page 1, Pre
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_40)    ;  "Choisissez les composants"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_41)    ;  "Choisissez les composants de $(LSTR_83) que vous souhaitez installer." "Bomberman ECE"
FunctionEnd


Function func_15    ; Page 1, Show
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1042
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_42)    ;  Description
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1043
  EnableWindow $_0_ 0
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1043
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_43)    ;  "Passer le curseur de votre souris sur un composant pour voir sa description."
  StrCpy $_4_ $(LSTR_43)    ;  "Passer le curseur de votre souris sur un composant pour voir sa description."
FunctionEnd


Function func_26    ; Page 1, Leave
FunctionEnd


Function func_27    ; Page 2, Pre
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_44)    ;  "Choisissez le dossier d'installation"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_45)    ;  "Choisissez le dossier dans lequel installer $(LSTR_83)." "Bomberman ECE"
FunctionEnd


Function func_32    ; Page 2, Show
FunctionEnd


Function func_33    ; Page 2, Leave
FunctionEnd


Function func_34    ; Page 3, Pre
  StrCmp $_3_ "" 0 label_38
  ReadRegStr $_0_ HKCU "Software\Bomberman ECE" "Bomberman ECE"
  StrCmp $_0_ "" label_38
  StrCpy $_3_ $_0_
label_38:
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_46)    ;  "Choisissez un dossier dans le menu Démarrer"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_47)    ;  "Choisissez un dossier dans le menu Démarrer pour les raccourcis de l'application."
  StrCmp $(LSTR_48) 0 label_58    ;  0
  StartMenu::Init /NOUNLOAD /rtl /noicon /autoadd /text $(LSTR_51) /lastused $_3_ /checknoshortcuts $(LSTR_50) $(LSTR_2)    ;  "Choisissez le dossier du menu Démarrer dans lequel vous voulez placer les raccourcis du programme. Vous pouvez également entrer un nouveau nom pour créer un nouveau dossier." "Ne pas créer de raccourcis" "Bomberman ECE"
    ; Call func_353
    ; SetOverwrite off
    ; File $PLUGINSDIR\StartMenu.dll
    ; DetailPrint ""
    ; Push $(LSTR_2)    ;  "Bomberman ECE"
    ; Push $(LSTR_50)    ;  "Ne pas créer de raccourcis"
    ; Push /checknoshortcuts
    ; Push $_3_
    ; Push /lastused
    ; Push $(LSTR_51)    ;  "Choisissez le dossier du menu Démarrer dans lequel vous voulez placer les raccourcis du programme. Vous pouvez également entrer un nouveau nom pour créer un nouveau dossier."
    ; Push /text
    ; Push /autoadd
    ; Push /noicon
    ; Push /rtl
    ; CallInstDLL $PLUGINSDIR\StartMenu.dll /NOUNLOAD Init
  Goto label_71
label_58:
  StartMenu::Init /NOUNLOAD /noicon /autoadd /text $(LSTR_51) /lastused $_3_ /checknoshortcuts $(LSTR_50) $(LSTR_2)    ;  "Choisissez le dossier du menu Démarrer dans lequel vous voulez placer les raccourcis du programme. Vous pouvez également entrer un nouveau nom pour créer un nouveau dossier." "Ne pas créer de raccourcis" "Bomberman ECE"
    ; Call func_353
    ; AllowSkipFiles off
    ; File $PLUGINSDIR\StartMenu.dll
    ; DetailPrint ""
    ; Push $(LSTR_2)    ;  "Bomberman ECE"
    ; Push $(LSTR_50)    ;  "Ne pas créer de raccourcis"
    ; Push /checknoshortcuts
    ; Push $_3_
    ; Push /lastused
    ; Push $(LSTR_51)    ;  "Choisissez le dossier du menu Démarrer dans lequel vous voulez placer les raccourcis du programme. Vous pouvez également entrer un nouveau nom pour créer un nouveau dossier."
    ; Push /text
    ; Push /autoadd
    ; Push /noicon
    ; CallInstDLL $PLUGINSDIR\StartMenu.dll /NOUNLOAD Init
label_71:
  Pop $_5_
  StartMenu::Show
    ; Call func_353
    ; AllowSkipFiles on
    ; File $PLUGINSDIR\StartMenu.dll
    ; DetailPrint ""
    ; CallInstDLL $PLUGINSDIR\StartMenu.dll Show
  Pop $_0_
  StrCmp $_0_ success 0 label_79
  Pop $_3_
label_79:
FunctionEnd


Function func_80    ; Page 3, Leave
FunctionEnd


Function func_81    ; Page 4, Pre
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_52)    ;  "Installation en cours"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_53)    ;  "Veuillez patienter pendant que $(LSTR_83) est en train d'être installé." "Bomberman ECE"
FunctionEnd


Function func_86    ; Page 4, Show
FunctionEnd


Function func_87    ; Page 4, Leave
  IfAbort label_93
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_54)    ;  "Installation terminée"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_55)    ;  "L'installation s'est terminée avec succès."
  Goto label_97
label_93:
  GetDlgItem $_0_ $HWNDPARENT 1037
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_56)    ;  "Installation interrompue"
  GetDlgItem $_0_ $HWNDPARENT 1038
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_57)    ;  "L'installation n'a pas été terminée."
label_97:
  IfAbort label_98
label_98:
FunctionEnd


Function .onGUIInit
  Call func_353
  DetailPrint ""
  SetOverwrite on
  File $PLUGINSDIR\modern-header.bmp
  SetBrandingImage /IMGID=1046 /RESIZETOFIT $PLUGINSDIR\modern-header.bmp
  GetDlgItem $_0_ $HWNDPARENT 1037
  CreateFont $_1_ $(LSTR_58) $(LSTR_59) 700    ;  "MS Shell Dlg" 8
  SendMessage $_0_ ${WM_SETFONT} $_1_ 0
  SetCtlColors $_0_ "" 0xFFFFFF
  GetDlgItem $_0_ $HWNDPARENT 1038
  SetCtlColors $_0_ "" 0xFFFFFF
  GetDlgItem $_0_ $HWNDPARENT 1034
  SetCtlColors $_0_ "" 0xFFFFFF
  GetDlgItem $_0_ $HWNDPARENT 1039
  SetCtlColors $_0_ "" 0xFFFFFF
  GetDlgItem $_0_ $HWNDPARENT 1028
  SetCtlColors $_0_ /BRANDING ""
  GetDlgItem $_0_ $HWNDPARENT 1256
  SetCtlColors $_0_ /BRANDING ""
  SendMessage $_0_ ${WM_SETTEXT} 0 "STR:$(LSTR_0) "    ;  "Nullsoft Install System v2.16"
FunctionEnd


Function .onUserAbort
  MessageBox MB_YESNO|MB_ICONEXCLAMATION $(LSTR_60) IDYES label_121    ;  "Êtes-vous sûr de vouloir quitter l'installation de $(LSTR_2) ?" "Bomberman ECE"
  Abort
label_121:
FunctionEnd


Section "Bomberman ECE" ; Section_0
  ; AddSize 8087
  SetOutPath $INSTDIR
  File bomberman.exe
  StrCpy $_OUTDIR $OUTDIR
  File alleg42.dll
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  SetOutPath $_OUTDIR\images
  File bombe_0.bmp
  File bombe_1.bmp
  File bombe_2.bmp
  File bombe_3.bmp
  File bonus_bombes.bmp
  File bonus_explosion.bmp
  File bonus_sante.bmp
  File bonus_vitesse.bmp
  File caisse.bmp
  File deux_joueurs.bmp
  File deux_joueurs_hover.bmp
  File diamant.bmp
  File editeur.bmp
  File editeur_hover.bmp
  File end_door.bmp
  File explosion_0.bmp
  File explosion_1.bmp
  File explosion_2.bmp
  File explosion_3.bmp
  File fond_highscores.bmp
  File game_over.bmp
  File highscores.bmp
  File highscores_hover.bmp
  File incassable.bmp
  File jeu.bmp
  File menu_principal.bmp
  File mort_0.bmp
  File mort_1.bmp
  File mort_2.bmp
  File mort_3.bmp
  File mort_4.bmp
  File pause.bmp
  File quitter.bmp
  File quitter_hover.bmp
  File sol.bmp
  File un_joueur.bmp
  File un_joueur_hover.bmp
  SetOutPath $_OUTDIR\images\editeur
  File fond.bmp
  File menu1.bmp
  File menu2.bmp
  SetOutPath $_OUTDIR\images\mechant
  File bas_0.bmp
  File bas_1.bmp
  File bas_2.bmp
  File bas_3.bmp
  File bas_4.bmp
  File droite_0.bmp
  File droite_1.bmp
  File droite_2.bmp
  File droite_3.bmp
  File droite_4.bmp
  File gauche_0.bmp
  File gauche_1.bmp
  File gauche_2.bmp
  File gauche_3.bmp
  File gauche_4.bmp
  File haut_0.bmp
  File haut_1.bmp
  File haut_2.bmp
  File haut_3.bmp
  File haut_4.bmp
  SetOutPath $_OUTDIR\images\player1
  File bas_0.bmp
  File bas_1.bmp
  File bas_2.bmp
  File bas_3.bmp
  File bas_4.bmp
  File droite_0.bmp
  File droite_1.bmp
  File droite_2.bmp
  File droite_3.bmp
  File droite_4.bmp
  File gauche_0.bmp
  File gauche_1.bmp
  File gauche_2.bmp
  File gauche_3.bmp
  File gauche_4.bmp
  File haut_0.bmp
  File haut_1.bmp
  File haut_2.bmp
  File haut_3.bmp
  File haut_4.bmp
  SetOutPath $_OUTDIR\images\player2
  File bas_0.bmp
  File bas_1.bmp
  File bas_2.bmp
  File bas_3.bmp
  File bas_4.bmp
  File droite_0.bmp
  File droite_1.bmp
  File droite_2.bmp
  File droite_3.bmp
  File droite_4.bmp
  File gauche_0.bmp
  File gauche_1.bmp
  File gauche_2.bmp
  File gauche_3.bmp
  File gauche_4.bmp
  File haut_0.bmp
  File haut_1.bmp
  File haut_2.bmp
  File haut_3.bmp
  File haut_4.bmp
  SetOutPath $_OUTDIR\installer
  File header.bmp
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  SetOutPath $_OUTDIR\sons
  File apparition_bonus.wav
  File bonus.wav
  File deflagration.wav
  File diamant.wav
  File explode.wav
  File gagne.wav
  File meurt.wav
  File mort_mechant.wav
  File perdu.wav
  File piege.wav
  File plant.wav
  File victory.wav
  File yessir.wav
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  SetOutPath $_OUTDIR\sons
  File musique.mid
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File police.dat
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  SetOutPath $_OUTDIR\niveaux
  File niveau1.niv
  File niveau10.niv
  File niveau11.niv
  File niveau12.niv
  File niveau13.niv
  File niveau14.niv
  File niveau2.niv
  File niveau3.niv
  File niveau4.niv
  File niveau5.niv
  File niveau6.niv
  File niveau7.niv
  File niveau8.niv
  File niveau9.niv
  SetOutPath $_OUTDIR
  WriteRegStr HKCU "Software\Bomberman ECE" "" $INSTDIR
  WriteUninstaller $INSTDIR\Desinstaller.exe ; 
  StrCpy $_0_ $_3_ 1
  StrCmp $_0_ > label_290
  StrCmp $_3_ "" 0 label_286
  ReadRegStr $_0_ HKCU "Software\Bomberman ECE" "Bomberman ECE"
  StrCmp $_0_ "" label_285
  StrCpy $_3_ $_0_
  Goto label_286
label_285:
  StrCpy $_3_ $(LSTR_2)    ;  "Bomberman ECE"
label_286:
  CreateDirectory $SMPROGRAMS\$_3_
  CreateShortCut $SMPROGRAMS\$_3_\Desinstaller.lnk $INSTDIR\Desinstaller.exe
  CreateShortCut "$SMPROGRAMS\$_3_\Jouer à bomberman.lnk" $INSTDIR\bomberman.exe
  WriteRegStr HKCU "Software\Bomberman ECE" "Bomberman ECE" $_3_
label_290:
SectionEnd


Section "Bomberman Sources" ; Section_1
  ; AddSize 169
  SetOutPath $INSTDIR
  StrCpy $_OUTDIR $OUTDIR
  File bomberman.dev
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File affichage.c
  File chargement_graphismes.c
  File chargement_niveaux.c
  File debug.c
  File editeur.c
  File gestion_highscore.c
  File gestion_objets.c
  File main.c
  File menu_principal.c
  File sons.c
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File affichage.h
  File bib.h
  File bomberman_private.h
  File chargement_graphismes.h
  File chargement_niveaux.h
  File debug.h
  File editeur.h
  File gestion_highscore.h
  File gestion_objets.h
  File menus.h
  File police.h
  File sons.h
  File types.h
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File bomberman_private.res
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File bomberman_private.rc
  SetOutPath $_OUTDIR
  StrCpy $_OUTDIR $OUTDIR
  File bomberman.ico
  File icone.ico
  SetOutPath $_OUTDIR
SectionEnd


Function .onMouseOverSection
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1043
  StrCmp $0 -1 0 label_342
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:
  EnableWindow $_0_ 0
  FindWindow $_0_ "#32770" "" $HWNDPARENT
  GetDlgItem $_0_ $_0_ 1043
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$_4_
  Goto label_352
label_342:
  StrCmp $0 0 0 label_347
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:
  EnableWindow $_0_ 1
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_61)    ;  "Le bomberman par défaut. Si vous le déselectionnez, vous n'installerez rien."
  Goto label_352
label_347:
  StrCmp $0 1 0 label_352
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:
  EnableWindow $_0_ 1
  SendMessage $_0_ ${WM_SETTEXT} 0 STR:$(LSTR_62)    ;  "Les sources de bomberman, vous avez besoin de Bomberman ECE pour pouvoir les utiliser."
  Goto label_352
label_352:
FunctionEnd


Function func_353
  DetailPrint ""
  StrCmp $PLUGINSDIR "" 0 label_363
  Push $0
  SetErrors
  GetTempFileName $0
  Delete $0
  CreateDirectory $0
  IfErrors label_364
  StrCpy $PLUGINSDIR $0
  Pop $0
label_363:
  Return

label_364:
  MessageBox MB_OK|MB_ICONSTOP "Error! Can't initialize plug-ins directory. Please try again later." /SD IDOK
  Quit
FunctionEnd



; --------------------
; UNREFERENCED STRINGS:

/*
*/
