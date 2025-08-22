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
  print "*         Script de compilation V2.0 pour Gamme Etendue\n";
  print "#*************************************************************************#\n";
  print "*\n";

  # Récupère le chemin et remplace les Slash par des AntiSlash  
  $APP_APP_FILE = "$ARGV[0]";
  $ProjectDirectory = "$ARGV[1]";
  $ProjectDirectory =~ s/\//\\/g ;
  $APP_DIRECTORY_DESTINATION = "$ProjectDirectory\\gcm";
  $APP_DIRECTORY_SOURCE = "$ProjectDirectory\\gcmF105L431";
  $File1 = "$ARGV[2]";
  $File2 = "$ARGV[3]";
  $APP_DIRECTORY = "$ProjectDirectory\\mercura\\src\\App";
  
   print "1- $APP_APP_FILE \n" ;  
   print "2- $ProjectDirectory \n" ;
   print "3- $APP_DIRECTORY_SOURCE \n" ;
   print "4- $APP_DIRECTORY_DESTINATION \n" ; 
   print "5- $File1 \n";  
   print "6- $File2 \n"; 
   
  # 1 lecture des infos du fichier APP.c
  if(! open(APP_FILE, "<$APP_DIRECTORY\\$APP_APP_FILE"))
  {
    print "* ** Impossible d'ouvrir le fichier app ** \n";
    system("pause");
    die "*\n#*************************************************************************#\n";  
  }

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
       @HW_Name_List = m/\".{8}\"/g;
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
    }
  }
  
  print "7- $SW_Name \n" ;  
  print "8- $HW_Name \n" ;  
  print "9- $SW_Version_WithoutPoint \n" ;  

 close(APP_FILE);
 
 # 2 ouverture ou creation du fichier GCM
 if (! open(GCM_FILE, "+>$APP_DIRECTORY_DESTINATION\\$SW_Name$SW_Version_WithoutPoint\.gcm")) 
 {
    print "* ** Impossible d'ouvrir le fichier file gcm ** \n";
    system("pause");
   die "*\n#*************************************************************************#\n";  
 }
  
 # 3 ouverture du fichier GCM file1
 if (! open(GCM_FILE_1, "$APP_DIRECTORY_SOURCE\\$SW_Name$SW_Version_WithoutPoint$File1\.gcm")) 
 {
    print "* ** Impossible d'ouvrir le fichier file 1 ** \n";
    system("pause");
   die "*\n#*************************************************************************#\n";  
 }
 
 # 4 copie des donnees
 while(defined($LigneEnCours = <GCM_FILE_1>))
 {
	print GCM_FILE $LigneEnCours; 
 }
 close(GCM_FILE_1);
 
# 5 inserion du separateur de fichier DEBUT ZONE 2
# print  GCM_FILE "\n";
 print  GCM_FILE "\n";
 print  GCM_FILE "DEBUT ZONE 2";
 print  GCM_FILE "\n";
 print  GCM_FILE "\n";
 
 # 6 ouverture du fichier GCM file2
 if (! open(GCM_FILE_2, "$APP_DIRECTORY_SOURCE\\$SW_Name$SW_Version_WithoutPoint$File2\.gcm")) 
 {
    print "* ** Impossible d'ouvrir le fichier file 2 ** \n";
    system("pause");
   die "*\n#*************************************************************************#\n";  
 }
 
 # 4 copie des donnees
 while(defined($LigneEnCours = <GCM_FILE_2>))
 {
	print GCM_FILE $LigneEnCours; 
 }
 close(GCM_FILE_2);
 
 close(GCM_FILE);
  #system("pause");
  

  
