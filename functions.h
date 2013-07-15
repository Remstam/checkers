//�� ����� ���� ����� ������ ���� ������ ��������
#include "variables.h"

/*====================FUNCTION DESCRIPTION BEGINS============*/
void draw_string(void* font, const char* sz, float x, float y);		/*�������� ����� ������ ����� �� �����*/
void draw_quad(const GLfloat *params, int shiftx, int shifty);		/*������������ ������ ������ ������*/
void mouse_button(int _button, int state, int x, int y);
void mouse_move(int x, int y);										/*���������� �������� ���� � ������� �������*/
void on_size(int w, int h);											/*���������� ��������� �������� ������*/
void ppd(GLfloat x, GLfloat y, GLfloat z);							/*������ �����*/
void put_checkers();												/*������ �� ��� ����� � ������������ � ������*/
void put_quads();													/*������ ������ � ������ �����*/
void texture_from_file(const char* filename, GLuint id);			/*�������� ����� � ��������� �������*/
/*====================FUNCTION DESCRIPTION ENDS==============*/

extern "C"
	char* read_bmp(const char* filename, int *width, int *height, int *components);

void draw_string(void* font, const char* sz, float x, float y)
{
	if (!sz) return;										// ���� ������� 0, ������ ������ - �������
	glRasterPos2f(x,y);										// ��������� ������� ������
	while(*sz) {glutBitmapCharacter(font, *sz); sz++; }		// ������ ��������� �����������
}

void draw_quad(const GLfloat *params, int shiftx, int shifty)
{
	glPushMatrix();
		glTranslatef(((cellx+shiftx)*2-7)*step,((celly+shifty)*2-7)*step,0);
		glBegin(GL_QUADS);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,params);
			glVertex3f(-step,step,0.001);
			glVertex3f(-step,-step,0.001);
			glVertex3f(step,-step,0.001);
			glVertex3f(step,step,0.001);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
		glEnd();
	glPopMatrix();
}

void mouse_button(int _button, int state, int x, int y)
{
	if(state==1)
	{						// 0 - ������ �� ������, 1 - ��������� ������
		button=-1;			// ������� ������ �� ������
		if(_button==0) place=((pixel>>16)-1)/2;//���������� ��������������
		return;
	}

	switch(button=_button)
	{
		case 0:				//�����
				vx=x;
				vy=y;
				oldcellx=cellx;
				oldcelly=celly;
				stemp=0;
				break;
		case 1:				//�������
				oldy=y;
				break;
		case 2:				//������
				oldx=x;
				oldy=y;
				break;
		default: break;
	}
}

void mouse_move(int x, int y)
{
	switch(button)
	{
		case 1:
				distZ+=((float)(y-oldy))/100;
				oldy=y;
				break;
		case 2:
				angleX+=x-oldx;
				angleY+=y-oldy;

				oldx=x;
				oldy=y;
				break;
		default: break;
	}
}

void on_size(int w, int h)
{
	width=w;
	height=h;
	if(height==0) height=1;		// �� ������ ������� �� 0
}

void ppd(GLfloat x, GLfloat y, GLfloat z)
{
	glBegin(GL_QUAD_STRIP);

		/*������� �����*/
		glTexCoord3d(1,0,0); glVertex3f(x/2,-y/2,0);
		glTexCoord3d(1,1,0); glVertex3f(x/2,y/2,0);
		glTexCoord3d(0,0,0); glVertex3f(-x/2,-y/2,0);
		glTexCoord3d(0,1,0); glVertex3f(-x/2,y/2,0);

		/*����� �����*/
		glVertex3f(-x/2,-y/2,-z);
		glVertex3f(-x/2,y/2,-z);

		/*������ �����*/
		glVertex3f(x/2,-y/2,-z);
		glVertex3f(x/2,y/2,-z);

		/*������ �����*/
		glVertex3f(x/2,-y/2,0);
		glVertex3f(x/2,y/2,0);
	glEnd();

	glBegin(GL_QUADS);
		/*������ �����*/
		glVertex3f(-x/2,-y/2,-z);
		glVertex3f(x/2,-y/2,-z);
		glVertex3f(x/2,-y/2,0);
		glVertex3f(-x/2,-y/2,0);

		/*�������� �����*/
		glVertex3f(-x/2,y/2,-z);
		glVertex3f(x/2,y/2,-z);
		glVertex3f(x/2,y/2,0);
		glVertex3f(-x/2,y/2,0);
	glEnd();
}

void put_checkers()
{
	int i,j;
	p = gluNewQuadric();

	glPushMatrix();
	glTranslatef(-7*step,-7*step,0);
	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
		{
			if(map[i][j]!=0)
			{
				glPushMatrix();
					glTranslatef(2*i*step,2*j*step,0);
					switch(map[i][j])
					{
						case 1: glMaterialfv(GL_FRONT,GL_DIFFUSE,yellow); break;
						case 2: glMaterialfv(GL_FRONT,GL_DIFFUSE,blue); break;
						default: break;
					}
				gluDisk(p,0,step,slice,slice);
				gluCylinder(p,step,step,0.02,slice,slice);
				glTranslatef(0,0,0.019);
				gluDisk(p,0,step,slice,slice);
				glutSolidTorus(0.01,step-0.01,slice,slice);
				glutSolidTorus(0.02,step-0.03,slice,slice);
				glutSolidCone(step-0.03,0.03,slice,slice);
				gluDisk(p,0,step-0.03,slice,slice);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void put_quads()
{
	int i,j;
	glPushMatrix();
		glTranslatef(-7*step,-7*step,0);
		for(j=0;j<8;j++)
			for(i=0;i<8;i++)
			{
				glPushMatrix();
					glTranslatef(i*2*step,j*2*step,0);
					glBegin(GL_QUADS);
						glColor3b(j*10+i,j*10+i,j*10+i);
						glVertex3f(-step,step,0);
						glVertex3f(-step,-step,0);
						glVertex3f(step,-step,0);
						glVertex3f(step,step,0);
					glEnd();
				glPopMatrix();
			}
	glPopMatrix();
}

void texture_from_file(const char* fname, GLuint id)
{
	int w=0,h=0,c=0;
	char* data=read_bmp(fname,&w,&h,&c);

	if(data==0) return;
	
	glBindTexture(GL_TEXTURE_2D,id);

	gluBuild2DMipmaps(GL_TEXTURE_2D,c,w,h,GL_BGR_EXT,GL_UNSIGNED_BYTE,data);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,(float)GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,(float)GL_REPEAT);
	free(data);
}