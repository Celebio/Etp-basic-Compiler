/*
    This file is part of ETP-Basic Compiler, a cross compiler for TI by Onur Celebi 2005-2007.

    ETP-Basic Compiler is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ETP-Basic Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tokenizer.h"

Tokenizer::Tokenizer(void)
{
    pListe=NULL;
}

Tokenizer::~Tokenizer(void)
{
}

void Tokenizer::Tokenize(char *textDeb,char *pFin,const char *fileName)
{
    programBuffer = textDeb;
    end = pFin;
    currentFileName = fileName;

    // Prend un text en entr?e et fourni un pointeur vers une liste chain?e de TAG
    TAG CourTAG;
    TAG* precpListeCr;
    char *pCour=programBuffer;
    int CourCol=0;
    int CourLigne=1;
    int precCol;

    pListeCr=pListe;
    precpListeCr=pListeCr;

    while (pListeCr)
    {
        precpListeCr=pListeCr;
        pListeCr=pListeCr->GetNext();
    }
    pListeCr=precpListeCr;

    while (pCour<end && *pCour)
    {
        do
        {
        precCol = CourCol;
        CourTAG = GetToken(&pCour,&CourCol);
        } while (CourTAG.GetToken() == TOKEN_ESPACE || CourTAG.GetToken() == TOKEN_COMMENT);

        if (CourTAG.GetToken()==TOKEN_UNKNOWN) continue;

        if (pListeCr)
        {
            pListeCr->SetNext(new TAG());
            pListeCr = pListeCr->GetNext();
        }
        else
        {
            pListeCr = new TAG();
            pListe = pListeCr;
        }

        *pListeCr = CourTAG;
        pListeCr->setLine(CourLigne);
        pListeCr->SetFileName(currentFileName);
        pListeCr->SetisPredefFileTAG(isPredefFile);
        pListeCr->SetCol(precCol);

        if (CourTAG.GetToken() == TOKEN_CRLF)
        {
            CourCol=0;
            CourLigne++;
        }
    }
    if (!pListeCr){
        pListeCr = new TAG();
    } else {
        pListeCr->SetNext(new TAG());
        pListeCr =pListeCr->GetNext();
    }
    pListeCr->SetCol(CourCol);
    pListeCr->setLine(0);
    pListeCr->SetToken(TOKEN_CRLF);
    pListeCr->SetFileName(currentFileName);
    pListeCr->SetisPredefFileTAG(isPredefFile);
    //pListeCr->SetNext(NULL);

    pListeCr->SetNext(new TAG());
    pListeCr =pListeCr->GetNext();
    pListeCr->SetCol(0);
    pListeCr->setLine(0);
    pListeCr->SetToken(TOKEN_ENDOFMODULE);
    pListeCr->SetFileName(currentFileName);
    pListeCr->SetisPredefFileTAG(isPredefFile);
    pListeCr->SetNext(NULL);


}

void Tokenizer::AvanceTokenPtr(char **pText)
{
    char jk;
    char InAString=0;
    jk = **pText;

    do
    {
        if (jk==34)
            InAString = (!InAString);
        (*pText)++;
        jk = **pText;
        if (jk=='.' && *((*pText)+1)>='0' && *((*pText)+1)<='9')
            AvanceTokenPtr(pText);

    } while (!(((jk== ' ' || jk == 9   || jk == 13   ||
                 jk== '+' || jk == '-' || jk == '*'  ||
                 jk== '/' || jk == '&' || jk == '<'  ||
                 jk== '=' || jk == '>' || jk == '('  ||
                 jk== ')' || jk == '[' || jk == ']'  ||
                 jk== ':' || jk == 39  || jk == ','  ||
                 jk== '.' || jk ==0   || jk == '!'  ||
                 jk== 34 || jk == 10 ) && (!InAString)) || (InAString && (jk==13 || jk==10)) ));    // les caract?res qui peuvent faire arreter un token
}

TAG Tokenizer::GetToken(char **bText,int* column)
{
    TAG perTAG;
    perTAG.SetIdentif(NULL);
    //perTAG.Identif[0] = 0;
    char* deb=*bText;
    if (**bText == 13 || **bText == 10)
    {
        (*bText)++;
        if (**bText == 10)
            (*bText)++; // il faut sauter le chr(10)
        perTAG.SetToken(TOKEN_CRLF);
    }
    else if (**bText == 32  || **bText == 9)    //espace
    {
        //avancer jusqu'a ne plus trouver d'espace

        while (**bText ==32 || **bText == 9)
        {
            if (**bText == 9)
                *column += TABSIZE;
            else if (**bText == 32)
                (*column)++;
            (*bText)++;
        }
        perTAG.SetToken(TOKEN_ESPACE);
    }
    else if (**bText == 39) // commentaire
    {
        //avancer jusqu'au retour de line
        while (**bText !=13 && **bText !=10 && **bText!=0)
            (*bText)++;
        perTAG.SetToken(TOKEN_COMMENT);
    }
    else if (**bText == ':')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_DEUXPOINTS);
    }
    else if (**bText == '.')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_POINT);
    }
    else if (**bText == '+')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_PLUS);
    }
    else if (**bText == '-')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_MINUS);
    }
    else if (**bText == '*')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_MULT);
    }
    else if (**bText == '/')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_DIV);
    }
    else if (**bText == '&')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_CONCAT);
    }
    else if (**bText == '!')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_EXCLAM);
    }
    else if (**bText == '(')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_OPENPAR);
    }
    else if (**bText == ')')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_CLOSEPAR);
    }
    else if (**bText == '[')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_OPENCRO);
    }
    else if (**bText == ']')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_CLOSECRO);
    }
    else if (**bText == ',')
    {
        (*bText)++;
        perTAG.SetToken(TOKEN_VIRGULE);
    }

    else if (**bText == '>')
    {
        (*bText)++;
        if (**bText == '=')
        {
            (*bText)++;
            perTAG.SetToken(TOKEN_SUPEQUAL1);
        }
        else
            perTAG.SetToken(TOKEN_SUPERIEUR);
    }
    else if (**bText == '<')
    {
        (*bText)++;
        if (**bText == '=')
        {
            (*bText)++;
            perTAG.SetToken(TOKEN_INFEQ1);
        }
        else if (**bText == '>')
        {
            (*bText)++;
            perTAG.SetToken(TOKEN_DIFFERENT);
        }
        else
            perTAG.SetToken(TOKEN_INF);
    }
    else if (**bText == '=')
    {
        (*bText)++;
        if (**bText == '<')
        {
            (*bText)++;
            // en toute rigeur: perTAG.Token = TOKEN_INFEQ2;
            perTAG.SetToken(TOKEN_INFEQ1);
        }
        else if (**bText == '>')
        {
            (*bText)++;
            // en toute rigeur: perTAG.Token = TOKEN_SUPEQUAL2;
            perTAG.SetToken(TOKEN_SUPEQUAL1);
        }
        else
            perTAG.SetToken(TOKEN_EQUAL);
    }
    else
    {
        //Voyons si c'est un reserved word
        char *deb=*bText;
        char *cPtr;
        const char *RW;
        AvanceTokenPtr(bText);

        int Found=0;
        int i=0;

        // comparer *cPtr avec ReservedWords[i] pour 0 =< i =<51
        while (!(Found || i>60))
        {
            RW=ReservedWords[i];
            Found = 1;
            cPtr=deb;   // rembobiner la cassette
            while ((*RW || cPtr!=(*bText)) && Found)
            {
                if (GiveLowerCase(RW)!=GiveLowerCase(cPtr))  // ou bien..  if ((*RW)!=(*cPtr))  pour respecter la casse
                    Found = 0;  // un caract?re est diff?rent.. donc c'est pas lui..
                else
                {
                    RW++;
                    cPtr++;
                }
            }
            i++;
        }


        if (Found)
            perTAG.SetToken(  (enumTokenType) (i-1) );
        else    // sinon ca fait pas partie des tokens, c'est donc un nombre ou un identifiant
        {
            //perTAG.SetIdentif((char *)malloc((size_t)((*bText)-deb)+1));
            //strncpy(perTAG.Identif,deb,(*bText)-deb);
            //memcpy(perTAG.GetIdentif(),deb,(*bText)-deb);
            char *aux = (char *)malloc((size_t)((*bText)-deb)+1);
            memcpy(aux,deb,(*bText)-deb);
            aux[(*bText)-deb]=0;
            perTAG.SetIdentif(aux);
            free(aux);

            //perTAG.GetIdentif()[(*bText)-deb]=0;
            if (IsNumeric(perTAG.GetIdentif())!=ISNOTNUMERIC){
                if (perTAG.GetIdentif()[0] == 0){
                    perTAG.SetToken(TOKEN_UNKNOWN);
                } else {
                    perTAG.SetToken(TOKEN_NOMBRE);
                }
            }
            else
            {
                if (*deb==34)
                {
                    perTAG.SetToken(TOKEN_STRINGCONSTANT);
                }
                else
                {
                    perTAG.SetToken(TOKEN_IDENTIF);
                    cPtr=perTAG.GetIdentif();
                    while (*cPtr)   // transformation en miniscule
                    {
                        *cPtr = GiveLowerCase(cPtr);
                        cPtr++;
                    }
                }
            }
        }
    }
    if (perTAG.GetToken() != TOKEN_ESPACE)
        *column += (int)((*bText)-deb);
    return perTAG;
}
char Tokenizer::GiveLowerCase(const char *bC)
{
    if (*bC>=65 && *bC<=90) // compris entre A et Z
        return *bC + 32;
    else
        return *bC;
}

errorIsNumeric Tokenizer::IsNumeric(char *ch)
{
    char *bText = ch;
    char PointNb = 0;
    while (*bText)  // en principe on ne recoit pas de string =""
    {
        if (!(*bText>='0' && *bText<='9'))
        {
            if (*bText == '.')
                PointNb++;
            else
                PointNb=2;  // pour qu'on retourne un ISNOTNUMERIC
        }
        bText++;
    }
    if (PointNb==0)
        return ISINTEGER;
    else if (PointNb==1)
        return ISFLOAT;
    else
        return ISNOTNUMERIC;
}
errorIsNumeric Tokenizer::IsNumeric(TAG* bTag)
{
    if (bTag->GetToken() != TOKEN_NOMBRE)
        return ISNOTNUMERIC;
    return IsNumeric(bTag->GetIdentif());
}
void Tokenizer::display(void)
{
    TAG* debut = pListe;
    while (debut)
    {
        debut->display();
        debut=debut->GetNext();
    }
}

