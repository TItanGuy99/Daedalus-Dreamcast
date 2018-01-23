#include "vector3.h"

#include <kos.h>
#include <math.h>

extern "C" float vec_length_sqrt( float *v );
extern "C" void vec_length( float *v0 );
extern "C" void  vec_normalize( float *v );
extern "C" float vec_dot( float *v0, float *v1 );
static tVector3 sm __attribute__((aligned(32)));
/*
 * Initialisierungskonstruktor
 * x: X-Komponente des Vektors
 * y: Y-Komponente des Vektors
 * z: Z-Komponente des Vektors
 */
tVector3::tVector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

/*
 * Destruktor
 */
tVector3::~tVector3()
{
}

/*
 * �berladener Vergleichsoperator, welcher zwei Vektoren auf Gleichheit �berpr�ft
 * other: Vektor, mit dem der linksseitige Operant verglichen wird
 * returns: true, bei Gleichheit der Objekte, ansonsten false
 */
bool tVector3::operator == ( tVector3 other )
{
	if( other.x == x && other.y == y && other.z == z )
		return true;
	else
		return false;
}

/*
 * �berladener + Operator, welcher zwei Vektoren addiert
 * other: Vektor, der zum linksseitigen Vektor addirt wird
 * returns: Addierter Vektor
 */
tVector3 tVector3::operator + ( const tVector3 &rhs ) const
{
	// Ergebnisvektor zur�ckgeben
	return tVector3( x + rhs.x, y + rhs.y, z + rhs.z );
}

/*
 * �berladener + Operator, welcher zwei Vektoren addiert
 * other: Vektor, der zum linksseitigen Vektor addiert wird
 */
void tVector3::operator += ( tVector3 other )
{
	// Berechnung durchf�hren
	x += other.x;
	y += other.y;
	z += other.z;
}

/*
 * �berladener - Operator, welcher den Vektor invertiert
 * returns: Invertierter Vektor
 */
tVector3 tVector3::operator - () const
{
	return tVector3( -x, -y, -z );
}

/*
 * �berladener - Operator, welcher zwei Vektoren subtrahiert
 * other: Vektor, der vom linksseitigen Vektor abgezogen wird
 * returns: Subrahierten Vektor
 */
tVector3 tVector3::operator - ( const tVector3 &rhs ) const
{
	// Ergebnisvektor zur�ckgeben
	return tVector3( x - rhs.x, y - rhs.y, z - rhs.z );
}

/*
 * �berladener * Operator, welcher einen Vektor mit einem Faktor multipliziert
 * other: Faktor, mit welchen der linkseitige Vektor multipliziert wird
 * returns: Multiplizierter Vektor
 */
tVector3 tVector3::operator * ( const float rhs ) const
{
	// Ergebnisvektor zur�ckgeben
	return tVector3( x * rhs, y * rhs, z * rhs );
}

/*
 * �berladener / Operator, welcher einen Vektor durch einen Faktor dividiert
 * other: Faktor, durch welchen der linkseitige Vektor dividiert wird
 * returns: Dividierter Vektor
 */
tVector3 tVector3::operator / ( float f )
{
	// Ergebnisvektor zur�ckgeben
	return (*this) * ( 1.0f / f );
}

/*
 * �berladener [] Operator f�r den Zugriff auf die einzelnen Komponenten des Vektors
 * iIndex: Index des Elements
 * returns: Wert des Elements mit dem angegebenen Index
 */
float& tVector3::operator [] ( int iIndex )
{
	return ((float *)this)[iIndex];
}
#define vector_normalize(x, y, z ) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fmov	fr0, fr4\n" \
    	"fmov	fr1, fr5\n" \
	    "fmov	fr2, fr6\n" \
        "fldi0  fr7\n" \
       	"fipr	fv4, fv4\n" \
	    "fmov	fr7, fr0\n" \
	    ".short	0xf07d\n" \
        "fmov	fr0, fr7\n" \
        "fmov	fr4, fr0\n" \
	    "fmul	fr7, fr0\n" \
	    "fmul	fr7, fr1\n" \
	    "fmul	fr7, fr2\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x; y = __y; z = __z; \
}

#define vector_length(x, y, z, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (0); \
	__asm__ __volatile__( \
       	"fipr	fv0, fv0\n" \
	    "fsqrt	fr3\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__d) \
		: "fr3" ); \
	d = __d; \
}

#define vector_length_sqrt(x, y, z, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (0); \
	__asm__ __volatile__( \
       	"fipr	fv0, fv0\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__d) \
		: "fr3" ); \
	d = __d; \
}

/*
 * Berechnet das Produkt zweier Vektoren
 * other: Zweiter Vektor
 * returns: Produkt zweier Vektoren
 */
float tVector3::dot( tVector3 *other )
{

  	return x * other->x + y * other->y + z * other->z;
  	/*float erg = other.z;
  	vector_dot(x, y, z, other.x, other.y, erg );
  	return erg;*/
  	//return vec_dot( (float *)&other, (float *)this );
  /*	register float __x1 __asm__("fr0") = (x);
  	register float __y1 __asm__("fr1") = (y);
  	register float __z1 __asm__("fr2") = (z);
  	register float __w1 __asm__("fr3") = (0);
  	
  	register float __x2 __asm__("fr4") = (other.x);
  	register float __y2 __asm__("fr5") = (other.y);
  	register float __z2 __asm__("fr6") = (other.z);
  	register float __w2 __asm__("fr7") = (0);
  	
  	__asm__ __volatile__( "
	   fldi0	fr3

	   fipr		fv0, fv4"
	);
	
    float x0 = __w2;
    __y1 = __z1 = __x2 = __y2 = __z2 = __x1 = __x1 = __w1;
    return x0;*/
}

/*
 * Berechnet das Kreuz-Produkt zweier Vektoren
 * other: Zweiter Vektor
 * returns: Kreuz-Produkt zweier Vektoren
 */
tVector3 tVector3::cross( tVector3 *other )
{
	// Speichert den Ergebnisvektor
	tVector3 erg;
	
	// Berechnung durchf�hren
	erg.x = y * other->z - z * other->y;
	erg.y = z * other->x - x * other->z;
	erg.z = x * other->y - y * other->x;
		
	// Ergebnisvektor zur�ckgeben
	return erg;
}

/*
 * Gibt die L�nge des Vektors zur�ck
 * returns: L�nge des Vektors
 */
float tVector3::length()
{
	return fsqrt( x * x + y * y + z * z );
/*	float erg = 0;
	vector_length( x, y, z, erg );
	return erg;*/
	/*memcpy( &sm, &x, sizeof(tVector3) );
	vec_length( (float *)&sm );
	return sm.z;*/
}

/*
 * Gibt die quadrierte L�nge des Vektors zur�ck
 * returns: Quadrierte L�nge des Vektors
 * remarks: Schneller als die length-Methode
 */
float tVector3::lengthSQRT()
{
	return x * x + y * y + z * z;
/*	float erg = 0;
	vector_length_sqrt( x, y, z, erg );
	return erg;*/
   // return vec_length_sqrt( &x );
}

/*
 * Normalisiert den Vektor
 * returns: Normalisierter Vektor
 */
tVector3 tVector3::normalize()
{
	// L�nge des Vektors ermitteln
/*	float len = this->lengthSQRT();

	// Sollte die L�nge 0 sein, oder der Vektor ist bereits normalisiert, dann abbrechen
	if( len == 0.0f || len == 1.0f )
		return (*this);
		
	return (*this) * frsqrt( len );*/
	
    tVector3 erg( x, y, z);
    vector_normalize( erg.x, erg.y, erg.z );
//    vec_normalize( (float *)&erg );
    return erg;
}

/*
 * Berechnet den Punkt auf der Linie, welcher dem vom Vektor repr�sentierten Punkt am n�hsten liegt
 * a: Vektor, welcher den Startpunkt der Linie darstellt
 * b: Vektor, welcher den Endpunkt der Linie darstellt
 * returns: Punkt auf der Linie, welcher dem vom Vektor repr�sentierten Punkt am n�hsten liegt
 */
tVector3 tVector3::closestPointOnLine( tVector3 *a, tVector3 *b )
{
	// t berechnen
	tVector3 c = (*this) - (*a);
	tVector3 ba = (*b) - (*a);
	float d = ba.length();
	tVector3 V = ba * ( 1.0f / d );
	float t = V.dot( &c );
	
	// �berpr�fen, ob t auf der Linie von a nach b liegt
	if( t < 0.0f ) 
		return (*a);
	if( t > d ) 
		return (*b);
 
   // Wenn ja, dann den berechneten Punkt zwischen a und b zur�ckgeben
   return (*a) + V * t;
}

/*
 * Berechnet den Punkt auf der Linie, welcher dem vom Vektor repr�sentierten Punkt am n�hsten liegt
 * a: Vektor, welcher den Startpunkt der Linie darstellt
 * b: Vektor, welcher den Endpunkt der Linie darstellt
 * d: L�nge der Linie
 * returns: Punkt auf der Linie, welcher dem vom Vektor repr�sentierten Punkt am n�hsten liegt
 * remarks: Schneller als ohne die d-Komponente, da die L�nge oft vorberechnete gespeichert werden kann
 */
tVector3 tVector3::closestPointOnLine( tVector3 *a, tVector3 *b, float d )
{
	// t berechnen
	tVector3 c = (*this) - (*a);
	tVector3 V = ((*b) - (*a)) / d;
	float t = V.dot( &c );

	// �berpr�fen, ob t auf der Linie von a nach b liegt
	if( t < 0.0f )
		return (*a);
	if( t > d )
		return (*b);

   // Wenn ja, dann den berechneten Punkt zwischen a und b zur�ckgeben
   return (*a) + V * t;
}

/*
 * Verschiebt den Vektor um die angegebene Matrix
 * m: Matrix, durch welche der Vektor verschoben wird
 * returns: Verschobener Vektor
 */
tVector3 tVector3::translateCoord( tMatrix m )
{
	// Vektor verschieben und zur�ckgeben	
	return tVector3( x + m.m[3][0], y + m.m[3][1], z + m.m[3][2] );
}

/*
 * Verschiebt den invertierten Vektor um die angegebene Matrix
 * m: Matrix, durch welche der invertierte Vektor verschoben wird
 * returns: Verschobener Vektor
 */
tVector3 tVector3::inverseTranslateCoord( tMatrix m )
{
	// Vektor verschieben und zur�ckgeben	
	return tVector3( x - m.m[3][0], y - m.m[3][1], z - m.m[3][2] );
}

/*
 * Rotiert den Vektor um die angegebene Matrix
 * m: Matrix, durch welche der Vektor rotiert wird
 * returns: Rotierter Vektor
 */
tVector3 tVector3::rotateCoord( tMatrix m )
{
	// Speichert den Ergebnisvektor
	tVector3 erg;
	
	erg.x = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z;
	erg.y = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z;
	erg.z = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z;
	
	// Ergebnisvektor zur�ckgeben
	return erg;
}

/*
 * Rotiert den invertierten Vektor um die angegebene Matrix
 * m: Matrix, durch welche der invertierte Vektor rotiert wird
 * returns: Rotierter Vektor
 */
tVector3 tVector3::inverseRotateCoord( tMatrix m )
{
	// Speichert den Ergebnisvektor
	tVector3 erg;
	
	erg.x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z;
	erg.y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z;
	erg.z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z;
	
	// Ergebnisvektor zur�ckgeben
	return erg;
}

/*
 * Transformiert den Vektor durch die angegebene Matrix
 * m: Matrix, durch welche der Vektor transformiert wird
 * returns: Transformierter Vektor
 */
tVector3 tVector3::transformCoord( tMatrix m )
{
	// Vektor verschieben
	tVector3 erg = rotateCoord( m );
	
	// Vektor rotieren
	erg = erg.translateCoord( m );	
   
	// Ergebnis zur�ckgeben
	return erg;
}

/*
 * Transformiert die Normale durch die angegebene Matrix
 * m: Matrix, durch welche die Normale transformiert wird
 * returns: Transformierte Normale
 */
tVector3 tVector3::transformNormal( tMatrix m )
{
	// Vektor verschieben
	tVector3 erg = rotateCoord( m );

	// Ergebnis zur�ckgeben
	return erg;
}

/*
 * Transformiert den invertierten Vektor durch die angegebene Matrix
 * m: Matrix, durch welche der invertierte Vektor transformiert wird
 * returns: Transformierter Vektor
 */
tVector3 tVector3::inverseTransformCoord( tMatrix m )
{
	// Vektor verschieben
	tVector3 erg = inverseTranslateCoord( m );
		
	// Vektor rotieren
	erg = erg.inverseRotateCoord( m );	
			
	// Ergebnis zur�ckgeben
	return erg;
}
