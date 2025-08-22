#!/usr/bin/perl

#*******************************************************************************
#             MERCURA Departement SYNERGIE Ingénierie Industrielle             *
#                                                                              *
#                Ce document est la propriétée de la société                   *
#            MERCURA Département SYNERGIE Ingénierie Industrielle              *
#     Il ne doit pas être copié partiellement ou totalement sans l'accord      *
#                       écrit de la société MERCURA                            *
#*******************************************************************************
#
# Version V1.0 : Création du script de compilation.
#  - Lit les variables du fichier APP.C
#  - Regénère le fichier APP.C avec les variables
#  - Calcul le checksum
#  - Génère le fichier GCM
#  - Génère le fichier A2L en fonction du fichier T55 et du fichier MAP
#
#*******************************************************************************

# Fonction ASCIIToInt
# - S'apelle de la façon suivante : ASCIIToInt (ByteMSB, ByteLSB)
# - Elle permet de transformer deux caractères héxadécimaux pouvant chacun prendre
# pour valeur 0 à F en valeur entière décimale comprise entre 0 et 255.
sub ASCIIToInt
{
  ($ByteMSB,$ByteLSB) = @_;
  $ValueToReturn = 0;
  $ASCII_A = "A";
  $ASCII_B = "B";
  $ASCII_C = "C";
  $ASCII_D = "D";
  $ASCII_E = "E";
  $ASCII_F = "F";
    
  if ($ByteMSB eq $ASCII_A)
  {
    $ValueToReturn = 10; 
  }
  else
  {
    if ($ByteMSB eq $ASCII_B)
    {
      $ValueToReturn = 11; 
    }
    else
    {
      if ($ByteMSB eq $ASCII_C)
      {
        $ValueToReturn = 12; 
      }
      else
      {
        if ($ByteMSB eq $ASCII_D)
        {
          $ValueToReturn = 13; 
        }
        else
        {
          if ($ByteMSB eq $ASCII_E)
          {
            $ValueToReturn = 14; 
          }
          else
          {
            if ($ByteMSB eq $ASCII_F)
            {
              $ValueToReturn = 15; 
            }
            else
            {
              $ValueToReturn = $ByteMSB;
            }
          }        
        }      
      }
    }
  }
  $ValueToReturn *= 16;
  if ($ByteLSB eq $ASCII_A)
  {
    $ValueToReturn += 10; 
  }
  else
  {
    if ($ByteLSB eq $ASCII_B)
    {
      $ValueToReturn += 11; 
    }
    else
    {
      if ($ByteLSB eq $ASCII_C)
      {
        $ValueToReturn += 12; 
      }
      else
      {
        if ($ByteLSB eq $ASCII_D)
        {
          $ValueToReturn += 13; 
        }
        else
        {
          if ($ByteLSB eq $ASCII_E)
          {
            $ValueToReturn += 14; 
          }
          else
          {
            if ($ByteLSB eq $ASCII_F)
            {
              $ValueToReturn += 15; 
            }
            else
            {
              $ValueToReturn += $ByteLSB;
            }
          }        
        }      
      }
    }
  }
  return $ValueToReturn;  
}

# Fonction DecToHex
# - S'apelle de la façon suivante : DecToHex (Décimal)
# - Elle permet de transformer un nombre compris entre 0 et 15 en un caractère héxa
sub DecToHex
{
  ($Decimal) = @_;
  if ($Decimal < 10)
  {
    $U4 = $Decimal;
  }
  else
  {
    if ($Decimal == 10)
    {
      $U4 = A;
    }
    else
    {
      if ($Decimal == 11)
      {
        $U4 = B;
      }
      else
      {
        if ($Decimal == 12)
        {
          $U4 = C;
        }
        else
        {
          if ($Decimal == 13)
          {
            $U4 = D;
          }
          else
          {
            if ($Decimal == 14)
            {
              $U4 = E;
            }
            else
            {
              if ($Decimal == 15)
              {
                $U4 = F;
              }
              else
              {
                $U4 = $Decimal;
              }
            }
          }
        }
      }
    }  
  }
  return $U4;
}

sub Writebnf()
{    
	my($bnf2,$number2,$max2)= @_;
	
  chomp($bnf2);
  $bnf2=~tr/\:/ /;
  $bnf2=~tr/\,/ /;
	$max2 = $max2 + 1;
    
  print A2L_FILE  "/begin COMPU_METHOD CM_$number2 \"\" \n";
  print A2L_FILE  "  TAB_VERB \"%30.0\" \"\" \n";
  print A2L_FILE  "  COMPU_TAB_REF CVT_$number2 \n";
  print A2L_FILE  "/end COMPU_METHOD \n";
  print A2L_FILE  "\n";
  print A2L_FILE  "/begin COMPU_VTAB CVT_$number2 \"\" \n";
  #print A2L_FILE  "  TAB_VERB 2 $bnf2 \n";
  print A2L_FILE  "  TAB_VERB $max2 $bnf2 \n";
  print A2L_FILE  "/end COMPU_VTAB \n";
  print A2L_FILE  "\n";
}

sub WriteMea()
{
	my($name1,$adress1,$bnf1,$feature1,$unit1,$min1,$max1,$resolution1,$description1,$datatype1,$binpoint1,$number1)= @_;
	
  chomp($name1);
  chomp($adress1);
  chomp($bnf1);
  chomp($feature1);
  chomp($min1);
  chomp($max1);
  chomp($resolution1);
  chomp($description1);
  chomp($datatype1);
  chomp($binpoint1);
  chomp($number1);

  if($bnf1)
  {
    $cm1 = "CM_"+$number;	
  }
  else
  {
    if ($binpoint1=~m/BIN0/)
    {
      $cm1 = "CM_0";
    }
    if ($binpoint1=~m/BIN1/)
    {
      $cm1 = "CM_1"
    }
    if ($binpoint1=~m/BIN2/)
    {
      $cm1 = "CM_2";
    }
    if ($binpoint1=~m/BIN3/)
    {
      $cm1 = "CM_3";
    }
    if ($binpoint1=~m/BIN4/)
    {
      $cm1 = "CM_4";
    }
    if ($binpoint1=~m/BIN5/)
    {
      $cm1 = "CM_5";
    }
  }
  
	if ($datatype1=~m/U8/)
	{
		$type = "UBYTE";
		$type1 = "BYTE";
	}
	if ($datatype1=~m/S8/)
	{
		$type = "SBYTE";
		$type1 = "BYTE";
	}
	if ($datatype1=~m/U16/)
	{
		$type = "UWORD";
		$type1 = "WORD";
	}
	if ($datatype1=~m/S16/)
	{
		$type = "SWORD";
		$type1 = "WORD";
	}
	if ($datatype1=~m/U32/)
	{
		$type = "ULONG";
		$type1 = "LONG";
	}
	if ($datatype1=~m/S32/)
	{
		$type = "SLONG";
		$type1 = "LONG";
	}
	
	print A2L_FILE    "/begin MEASUREMENT $name1 \"$description1\" \n";
	print A2L_FILE    "  $type CM_$cm1 /* :1: */ 1 0 $min1 $max1 \n";
	printf A2L_FILE    "  ECU_ADDRESS 0x%X  \n",$adress1;
	print A2L_FILE    "/begin IF_DATA DIM \n";
	printf A2L_FILE    "  0x%X INTERN $type1 \n",$adress1;
	print A2L_FILE    "/end IF_DATA\n";
	print A2L_FILE    "/begin IF_DATA VISU\n";
	printf A2L_FILE    "  ADR_COM 0x%X ADR_ABS 0x%X\n",$adress1,$adress1;
	print A2L_FILE    "/end IF_DATA\n";
  print A2L_FILE    "/end MEASUREMENT\n";
  print A2L_FILE    "\n";
  
	
}

 print "#*************************************************************************#\n";
  print "*     MERCURA Département SYNERGIE Ingénierie Industrielle\n";
  print "*\n";
  print "*         Script de compilation V1.0 pour µControleur M16C\n";
  print "#*************************************************************************#\n";
  print "*\n";
  
  if ((scalar @ARGV != 5))
  {
    print "* BESOIN D'AIDE... Utiliser la synatxe suivante :\n";
  	print "* gene_app.pl <Project_Directory> <ToolKit_Directory> <Target_Directory>\n";
    print "*             <CanSpeed>\n";
    print "*\n";
  	print "*   <Project_Directory> : Repertoire de travail\n";
  	print "*   <ToolKit_Directory> : Repertoire des outils de compilation\n";
  	print "*   <Target_Directory>  : Repertoire de destination\n";
  	print "*   <CanSpeed>          : Vitesse de transmission\n";
    print "*\n";
    print "#*************************************************************************#\n";

    # Quitte le programme et retourne une erreur
    exit 1;
  }

  # Récupère le chemin et remplace les Slash par des AntiSlash  
  $ProjectDirectory = "$ARGV[0]";
  $ProjectDirectory =~ s/\//\\/g ;
  $APP_DIRECTORY = "$ProjectDirectory\\mercura\\src\\APP";
   print "$ProjectDirectory" ;  
  # system("pause");

  print "* Etape 1 : Generation du fichier app.c\n";
  print "*    - Debut de generation du fichier...\n";

# Lecture des infos nécessaires au flashage dans le fichier app.c
  if(! open(APP_FILE, "<$APP_DIRECTORY\\app\.c"))
  {
    print "* ** Impossible d'ouvrir le fichier app\.c ** \n";
    system("pause");
    die "*\n#*************************************************************************#\n";  
  }
  else
  {
  
  }

  #print "* ** ouverture de fichier Lecture fichier OK** \n";
  #system("pause");
  while(defined($LigneEnCours = <APP_FILE>))
  {
    if ($LigneEnCours=~/const\sU8\sAPP_SW_NAME/)
    {
      $_ = $LigneEnCours;
      @SW_Name_List = m/\"\w{4}\"/g;
      $SW_Name = $SW_Name_List[0];
      $SW_Name =~ s/\"//g;
    }
    if ($LigneEnCours=~/const\sU8\sAPP_HW_NAME/)
    {
      $_ = $LigneEnCours;
      @HW_Name_List = m/\"\w{8}\"/g;
      $HW_Name = $HW_Name_List[0];
      $HW_Name =~ s/\"//g;
    }
    if ($LigneEnCours=~/const\sU8\sAPP_SW_VERSION_TEXT/)
    {
      $_ = $LigneEnCours;
      @SW_Version_List = m/\"\w{2}.\w{2}\"/g;
      $SW_Version_WithPoint = $SW_Version_List[0];
      $SW_Version_WithPoint =~ s/\"//g;
      $SW_Version_WithoutPoint = $SW_Version_WithPoint;
      $SW_Version_WithoutPoint =~ s/\.//g;
      #print "SWVersion = $SW_Version_WithoutPoint";  
    }
    if ($LigneEnCours=~/const\sU8\sAPP_SW_VERSION_CAN_TEXT/)
    {
      $_ = $LigneEnCours;   
      @SW_Can_Version_List = m/{....}/g;
      $SW_Can_Version = $SW_Can_Version_List[0];
      #print "SWCAnVersion = $SW_Can_Version_WithPoint \n"; 
      #system("pause"); 
    }

    
  }
  
  close(APP_FILE);
  
  #Récupération du login de la session ouverte.
  $Utilisateur = Win32::LoginName();
# Récupération de la date et de l'heure courante.
  ($sec, $min, $hour, $mday, $month, $year, $wday, $aday, $isdst) = localtime(time);
# Mise en forme du jour si celui-ci est inférieur à 10.
  if (length($mday) == 1)
  {
    $mday = "0".$mday;
  }
# Mise en forme du mois si celui-ci est inférieur à 10.
  
  $month = $month+1;
  if (length($month) == 1)
  {
    $month = "0".$month;
  }
  else
  {
      #do nothing
  }
  
  
  
#Mise en forme de l'année.
  $year += 1900;
# Mise en forme de l'heure de compil si celle-ci est inférieure à 10.
  if (length($hour) == 1)
  {
    $hour = "0".$hour;
  }
# Mise en forme de la minute de compil si celle-ci est inférieure à 10.
  if (length($min) == 1)
  {
    $min = "0".$min;
  }
# Mise en forme de la seconde de compil si celle-ci est inférieure à 10.
  if (length($sec) == 1)
  {
    $sec = "0".$sec;
  }
  
# Mise en forme définitive de la ligne
  $App_Compil_Info_Long = "$SW_Name$SW_Version_WithoutPoint du $mday\/$month\/$year à $hour:$min:$sec"." "x33;
# Limitation de la chaine à 48 caractères
  ($App_Compil_Info_Court)=$App_Compil_Info_Long=~/(.{33}).*/;


($Week, $reste) = split(/\./, (($aday-$wday)/7));
$Week++;
if ($reste > 0)
{
	$Week ++;
}

$wday -= 1;
$year_light = $year - 2000;

# Creation et remplissage du fichier app.c
  if (! open(APP_FILE, ">$APP_DIRECTORY\\app\.c"))
  {
    print "* ** Impossible d'ouvrir le fichier app\.c ** \n";
    system("pause");
    die "*\n#*************************************************************************#\n";  
  }
  else
  {
  }
  
  print APP_FILE "\/*******************************************************************************\n";
  print APP_FILE "*             MERCURA Departement SYNERGIE Ingénierie Industrielle             *\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "*                Ce document est la propriétée de la société                   *\n";
  print APP_FILE "*            MERCURA Département SYNERGIE Ingénierie Industrielle              *\n";
  print APP_FILE "*     Il ne doit pas être copié partiellement ou totalement sans l'accord      *\n";
  print APP_FILE "*                       écrit de la société MERCURA                            *\n";
  print APP_FILE "*******************************************************************************\/\n";
  print APP_FILE "\n";
  print APP_FILE "\#include	\"types.h\" \n";
  print APP_FILE "\#include	\"app.h\"   \n";
  print APP_FILE "\n";
  print APP_FILE "\/*******************************************************************************\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "* ***                             ATTENTION !!!!!                          *** *\n";
  print APP_FILE "* ***           LA ZONE CI-DESSOUS EST UNE ZONE DE PARAMETRAGE             *** *\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "*  - SEULES LES VALEURS DES CONSTANTES PEUVENT ETRE MODIFIEES                  *\n";
  print APP_FILE "*  - TOUTE DONNEE SUPPLEMENTAIRE SERA AUTOMATIQUEMENT DETRUITE                 *\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "*------------------------------------------------------------------------------*\/\n";
  print APP_FILE "\n";
  print APP_FILE "\/\/ APP_SW_NAME ne doit pas dépasser 4 caractères HEXA\n";
  print APP_FILE "const U8 APP_SW_NAME[APP_SW_NAME_SIZE]\t\t\t= \"$SW_Name\";\n";
  print APP_FILE "\n";
  print APP_FILE "\/\/ APP_HW_VERSION TEXT doit absolument être formaté en 8 caractères\n";
  print APP_FILE "const U8 APP_HW_NAME[APP_HW_NAME_SIZE]\t\t\t= \"$HW_Name\";\n";
  print APP_FILE "\n";
  print APP_FILE "\/\/ APP_SW_VERSION TEXT doit absolument être formaté en xx.yy\n";
  print APP_FILE "const U8 APP_SW_VERSION_TEXT[APP_SW_VERSION_TEXT_SIZE]\t= \"$SW_Version_WithPoint\";\n";
  print APP_FILE "\n";
  print APP_FILE "\n";
  print APP_FILE "\/\/ APP_SW_VERSION TEXT doit absolument être formaté en xx.yy\n";
  print APP_FILE "const U8 APP_SW_VERSION_CAN_TEXT[APP_SW_VERSION_TEXT_CAN_SIZE]\t= $SW_Can_Version;\n";
  print APP_FILE "\n";
  print APP_FILE "\n";
  print APP_FILE "\/******************************************************************************\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "* ***                             ATTENTION !!!!                           *** *\n";
  print APP_FILE "* ***    LA ZONE CI-DESSOUS EST INTERDITE A TOUTE MISE A JOUR MANUELLE     *** *\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "*            TOUTE DONNEE SUPPLEMENTAIRE INSCRITE DANS CETTE ZONE              *\n"; 
  print APP_FILE "*                      SERA AUTOMATIQUEMENT DETRUITE                           *\n";
  print APP_FILE "*                                                                              *\n";
  print APP_FILE "*------------------------------------------------------------------------------*\/\n";
  print APP_FILE "\n";
  print APP_FILE "S8_CAL\t\tAPP_PROM_ID_APV[APP_PROM_ID_SIZE_CPV]\t\t= \"$App_Compil_Info_Court\";\n";
  print APP_FILE "const S8\tAPP_CODE_VERSION_CPV[APP_PROM_ID_SIZE_CPV]\t= \"$App_Compil_Info_Court\";\n";
  print APP_FILE "\n";
  print APP_FILE "S8_CAL\t\tAPP_USER_NAME_DATA_APV[APP_USER_NAME_DATA_SIZE_CPV] = \"$Utilisateur\";\n";
  print APP_FILE "const S8\tAPP_USER_NAME_CODE_CPV[APP_USER_NAME_CODE_SIZE_CPV] = \"$Utilisateur\";\n";
  print APP_FILE "\n";
  print APP_FILE "U8_CAL\t\tAPP_SW_VERSION_DATA_APV[APP_SW_VERSION_SIZE_CPV] = {0x$year_light, 0x$Week, 0x0$wday}; /* year, week, day in the week */ \n";
  print APP_FILE "const U8\tAPP_SW_VERSION_CODE_CPV[APP_SW_VERSION_SIZE_CPV] = {0x$year_light, 0x$Week, 0x0$wday}; /* year, week, day in the week */ \n";
  print APP_FILE "\n";
  
  
  close(APP_FILE);

print "*    - Fichier genere avec succes.\n";
# system("pause");
print "*\n* Etape 2 : Compilation du projet\n";
print "*\n* Veuillez lancer la compilation du projet et tapez :\n";
print "*\t <O> si la compilation s'est terminee avec succes\n";
print "*\t <N> si la compilation n'a pas aboutie\n";
print "*\tResultat : ";
$RepCompil = <STDIN>;
chomp $RepCompil;
$RepPositive = "O";
$RepCompil= "O";

if (uc($RepCompil) ne $RepPositive)
{
  print "* ** Fin du script de compilation ... Le fichier GCM ne sera pas genere!! **\n"; 
  exit 2;
}

 
  print "*\n* Etape 3 : Calcul du checksum de l'applicatif\n";
 # system("pause");
#Lancement du calcul de checksum du fichier issu de la compilation, puis préparation de la ligne I03
  $HexSourcefile="$ARGV[4]";
  $Target_Directory = "$ARGV[2]";
  
  opendir (Dir,$Target_Directory);
  ##@HexFiles = grep(/\.hex$/i, readdir AttacheDeTargetDir);
  if (! open(HEX_FILE, "$Target_Directory\\Release\\$HexSourcefile\.hex"))
  {
    print "* ** Impossible d'ouvrir le fichier $HexFiles[0]** \n";
    system("pause");
    die "*\n#*************************************************************************#\n";  
  }
  else
  {
  }
  $ChecksumDec = 0;
  
  $Locked = 0;
  
# on se possition au debut des data utile
#  $LignetestDebut = ":020000040802F0";
#  while(defined($LigneEnCours = <HEX_FILE>))
#  {  
#	if ($LignetestDebut=~substr($LigneEnCours,0,15))
#    {
#		last;
#	}
#  }

  while(defined($LigneEnCours = <HEX_FILE>))
  {  
    if ($LigneEnCours=~/^:/)
    {
            
      $Datarecordtype = substr($LigneEnCours,7,2);
            
      if($Datarecordtype=="04")      
      {
      $AdresseVal = substr($LigneEnCours,9,4);
        if($AdresseVal == "2000") 
        {  #debut RAM Adress, correction bug compilateur Gnu.
          $Locked = 1;  
        }
      }

      if($Datarecordtype=="00" && (0 == $Locked))
      {
        $NbOctetInLine = ASCIIToInt(substr($LigneEnCours,1,1), substr($LigneEnCours,2,1));

        for ($i=0;$i < $NbOctetInLine;$i++)
        {
        
        $CurrentMSB = substr($LigneEnCours,9+2*$i,1);
        $CurrentLSB = substr($LigneEnCours,10+2*$i,1);
        $CurrentOctet = ASCIIToInt(substr($LigneEnCours,9+2*$i,1), substr($LigneEnCours,10+2*$i,1));
      

          if ($CurrentOctet != 255)
          {
          $ChecksumDec += $CurrentOctet;
          $ChecksumDec++;
          }
        }
      }
    }
  }
  close(HEX_FILE);
  closedir (Dir);


  $Checksum_16b_Dec = $ChecksumDec % 65536;
  $ChecksumByte_1_MSB_Dec = int ($Checksum_16b_Dec / 4096); 
  $ChecksumByte_1_LSB_Dec = int (($Checksum_16b_Dec - ($ChecksumByte_1_MSB_Dec * 4096)) / 256);
  $ChecksumByte_0_MSB_Dec = int (($Checksum_16b_Dec - ($ChecksumByte_1_MSB_Dec * 4096) - ($ChecksumByte_1_LSB_Dec * 256)) / 16);
  $ChecksumByte_0_LSB_Dec = int (($Checksum_16b_Dec - ($ChecksumByte_1_MSB_Dec * 4096) - ($ChecksumByte_1_LSB_Dec * 256) - ($ChecksumByte_0_MSB_Dec * 16)));
  
  $ChecksumByte_1_MSB_Hex = DecToHex($ChecksumByte_1_MSB_Dec);
  $ChecksumByte_1_LSB_Hex = DecToHex($ChecksumByte_1_LSB_Dec);
  $ChecksumByte_0_MSB_Hex = DecToHex($ChecksumByte_0_MSB_Dec);
  $ChecksumByte_0_LSB_Hex = DecToHex($ChecksumByte_0_LSB_Dec);
  $ChecksumHex = "$ChecksumByte_1_MSB_Hex$ChecksumByte_1_LSB_Hex$ChecksumByte_0_MSB_Hex$ChecksumByte_0_LSB_Hex";
  
  print "*    - Checksum obtenu : $ChecksumHex\n";
 # system("pause");
  
  print "*\n* Etape 4 : Creation du fichier GCM\n";


if (! open(GCM_FILE, "+>$Target_Directory\\gcm\\$SW_Name$SW_Version_WithoutPoint\.gcm"))
{
  print "* ** Impossible d'ouvrir le fichier $Target_Directory\\$SW_Name$SW_Version_WithoutPoint\.gcm ** \n";
  system("pause");
  die "*\n#*************************************************************************#\n";  
}
else
{
 print "* **  ouverture : $SW_Name$SW_Version_WithoutPoint\.gcm ok** \n";
 # system("pause");
}

  print "*    - Renseignement des informations sur le SW (Champs I0x)\n";
#Préparation de la ligne I00
  $I00 = "I00INFORMATIONS SUR LE SW :\n";
  print GCM_FILE $I00;
#Préparation de la ligne I01
  $I01 = "I01 Nom du SW : $SW_Name$SW_Version_WithoutPoint\n";
  print GCM_FILE $I01;
#Préparation de la ligne I02
  $I02 = "I02   Version : V$SW_Version_WithPoint\n";
  print GCM_FILE $I02;
#Préparation de la ligne I03 
  $I03 = "I03  Checksum : $ChecksumHex\n";
  print GCM_FILE $I03;
  print GCM_FILE "\n";
#Préparation de la ligne I04 
  $I04 = "I04  Type GCM : 03\n";
  print GCM_FILE $I04;
  print GCM_FILE "\n";  
 
  print "*    - Renseignement des informations de compilation (Champs I1x)\n";
#Préparation de la ligne I10
  $I10 = "I10INFORMATIONS DE COMPILATION :\n";
  print GCM_FILE $I10;
#Préparation de la ligne I11
  $I11 = "I11 Compilé le : $mday\/$month\/$year\n";
  print GCM_FILE $I11;
#Préparation de la ligne I12
  $I12 = "I12          A : $hour:$min:$sec\n";
  print GCM_FILE $I12;
#Préparation de la ligne I13
  $I13 = "I13        Par : $Utilisateur\n";
  print GCM_FILE $I13;
#Préparation de la ligne I14
  $I14 = "I14  C.Version : V2.12A \n";
  print GCM_FILE $I14;
#Préparation de la ligne I15
  $I15 = "I15  L.Version : V4.56F \n";
  print GCM_FILE $I15;
  print GCM_FILE "\n";
  
  print "*    - Renseignement des informations de transmission (Champs I2x)\n";
#Préparation de la ligne I20
  $I20 = "I20INFORMATION DE TRANSMISSION\n";
  print GCM_FILE $I20;
#Préparation de la ligne I21
  $I21 = "I21   Protocole : PRC01\.00\n";
  print GCM_FILE $I21;
#Préparation de la ligne I22
  $CanSpeed = "$ARGV[3]";   
  $NbZeroAvCanSpeed = 7 - length($CanSpeed);
  $I22 = "I22 Vitesse CAN : "."0"x $NbZeroAvCanSpeed ."$CanSpeed\n";
  print GCM_FILE $I22;
#Préparation de la ligne I23
  $I23 = "I23       Id Tx : 0x001\n";
  print GCM_FILE $I23;
#Préparation de la ligne I24
  $I24 = "I24     Rx Mask : 0x40\n";
  print GCM_FILE $I24;
  print GCM_FILE "\n";

  print "*    - Renseignement des donnees de compilation\n";

  
  print "*------------------------------------    $HexSourcefile  --------------------------------\n";
#  system("pause");  
 
 #Remplissage du fichier GCM avec les données du fichier HEX
  if (! open(HEX_SOURCE_FILE, "$Target_Directory\\Release\\$HexSourcefile\.hex"))
  {
      print "* ** Impossible d'ouvrir le fichier $HexSourcefile** \n";
      system("pause");
          
  }
  else
  {
      print "Fichier ouvert $HexSourcefile** \n";
 #     system("pause");
          
  }
  
  #Copy le contenu du fichier hex dans le fichier gcm
  while(defined($LigneEnCours = <HEX_SOURCE_FILE>))
  {
    print GCM_FILE $LigneEnCours;
  }
  close (HEX_SOURCE_FILE);
  close(GCM_FILE);
  
  print "*    - Fichier GCM genere avec succes.\n";

  system("pause");
  
 

  
