#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <GL/glut.h>
#include <body.h>
#include <Config.h>


// Config Parameters
#define MAX_ID 20
#define SHOW_INTERVAL 1200
#define TR_INTERVAL  12
float   G_newton;
float   PI;
float   dt;
float   BODY_SIZE;
int     WIDTH = 1000;
int     HEIGHT = 1000;
int     BODY_NUM = 0;

// debug & test
#define SHOW_COORDINATE 1
#define SHOW_INFO 1

// Global Parameters
body    obj[MAX_ID];
Vec3d   track[MAX_ID][TR_INTERVAL];
int     cur[MAX_ID];
int     display_step=0;
GLfloat xangle,yangle,zangle,oldx,oldy;
int     SHIFT_R = 0, SHIFT_D = 0;


void read_cfg(void)
{
    char *cfg_file = "/home/dynmi/Documents/Project/N-Body-Gravity-Simulation/setting.cfg";
    printf("I am in baby\n");
    char key[1024] = {0}, *value;

    strcpy(key,"G_newton");
    readCFG(cfg_file/*in*/,key/*in*/, &value/*out*/);
    printf("%s\n",value);
    G_newton = atof(value);
    printf("%f\n",G_newton);

    strcpy(key,"PI");
    readCFG(cfg_file/*in*/,key/*in*/, &value/*out*/);
    printf("%s\n",value);
    PI = atof(value);
    printf("%f\n",PI);

    strcpy(key,"dt");
    readCFG(cfg_file/*in*/,key/*in*/, &value/*out*/);
    printf("%s\n",value);
    dt = atof(value);
    printf("%f\n",dt);

    strcpy(key,"BODY_SIZE");
    readCFG(cfg_file/*in*/,key/*in*/, &value/*out*/);
    printf("%s\n",value);
    BODY_SIZE = atof(value);  
    printf("%f\n",BODY_SIZE);

}


void cal_acc(int host, int sr)
{
    /**
     * calculate accelaration of obj[host] caused by obj[sr]
    **/

    float dis = sqrt(pow(obj[host].pos.x-obj[sr].pos.x, 2) 
                        + pow(obj[host].pos.y-obj[sr].pos.y, 2)
                        + pow(obj[host].pos.z-obj[sr].pos.z, 2));

    if(dis==0)
    {
        printf("error!!! distance can't be zero\n");
        return;
    }

    float base = G_newton * obj[sr].m / pow(dis,2);

    if(obj[sr].pos.x-obj[host].pos.x!=0)
    {
        obj[host].a.x += base * ((obj[sr].pos.x-obj[host].pos.x)/dis);
        // printf("x方向上， %d 受到%d %lf  %lf %f\n ", host, sr, obj[host].a.x, base, (obj[sr].pos.x-obj[host].pos.x)/dis);
    }

    if(obj[sr].pos.y-obj[host].pos.y!=0)
    {
        obj[host].a.y += base * ((obj[sr].pos.y-obj[host].pos.y)/dis);
    }

    if(obj[sr].pos.z-obj[host].pos.z!=0)
    {
        obj[host].a.z += base * ((obj[sr].pos.z-obj[host].pos.z)/dis);
    }

}


void update_body(int id)
{
    /**
     * update state of body i 
    **/
    
    // theta_x = v*t + 0.5*a*t*t
    float theta_x = obj[id].v.x * dt + 0.5 * obj[id].a.x *dt*dt;
    float theta_y = obj[id].v.y * dt + 0.5 * obj[id].a.y *dt*dt;
    float theta_z = obj[id].v.z * dt + 0.5 * obj[id].a.z *dt*dt;

    obj[id].pos.x +=theta_x + 1e-9f;
    obj[id].pos.y +=theta_y + 1e-9f;
    obj[id].pos.z +=theta_z + 1e-9f;

    obj[id].v.x +=obj[id].a.x *dt;
    obj[id].v.y +=obj[id].a.y *dt;
    obj[id].v.z +=obj[id].a.z *dt;

    track[id][cur[id]].x = obj[id].pos.x;
    track[id][cur[id]].y = obj[id].pos.y;
    track[id][cur[id]].z = obj[id].pos.z;
    cur[id] = (cur[id]+1)%TR_INTERVAL;
    pthread_exit(NULL);
}


void FreshBodies()
{
    /**
     * update positions for all bodies
    **/

    int i,j;
    // pthread_t tid[8];
    for(i=0; i<BODY_NUM; i++)
    {
        obj[i].a.x = 0;
        obj[i].a.y = 0;
        obj[i].a.z = 0;
    }

    for(i=0; i<BODY_NUM; i++)
    {
        for(j=0; j<BODY_NUM; j++)
        {
            if(j==i) continue;

            cal_acc(i,j);
        }
    }

    for(i=0; i<BODY_NUM; i++)
    {
        update_body(i);
        // int ret = pthread_create(&tid[i%8],NULL,update_body,i);
    }

}


float Pixel2Proportion(float sr, int axis)
{
    /**
     * transfer pixel coordinate to proportion coordinate
    **/

    if(axis==0)
        return (sr+SHIFT_R*4)/WIDTH*2;
    else if(axis==1)
        return sr/HEIGHT*2;
    else
        return (sr+SHIFT_D*4)/HEIGHT*2;
}


void DrawSphere(Vec3d position, float color, float R)
{
    glColor3f(color, 0.0f, 0.0f);
	glPushMatrix();
    glTranslatef(Pixel2Proportion(position.x,0), 
                Pixel2Proportion(position.y,1), 
                Pixel2Proportion(position.z,2));
    glutSolidSphere(R, 30,30);
    glPopMatrix();

    glEnd();
}


void drawCoordinates(void)
{
    // x axis
	glLineWidth(1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 0.0f);
	glEnd();
    glPushMatrix();
	glTranslated(0.8,0,0);
	glRotated(90, 0, 1, 0);
	glutSolidCone(0.01, 0.03, 20, 20);
    glPopMatrix();

    // y axis
    glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, 0.0f);
	glEnd();
    glPushMatrix();
	glTranslated(0,0.8,0);
	glRotated(-90, 1, 0, 0);
	glutSolidCone(0.01, 0.03, 20, 20);
    glPopMatrix();

    // z axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.8f);
	glEnd();
    glPushMatrix();
	glTranslated(0,0,0.8);
	glutSolidCone(0.01, 0.03, 20, 20);
    glPopMatrix();

}


void Display(void)
{
    // light source
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };  
    GLfloat mat_shininess[] = { 20.0 };               
    GLfloat light_position[] = { Pixel2Proportion(WIDTH,0), Pixel2Proportion(HEIGHT,1), Pixel2Proportion(HEIGHT,1), 0.0 };
    //GLfloat Light_Ambient[] = { xangle/360.0, yangle/360.0, zangle/360.0, 1.0 }; 
    GLfloat Light_Ambient[] = { 0.2, 0.8, 0.5, 1.0 }; 
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , Light_Ambient );  
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };   
    glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);  
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);   

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glRotatef(xangle, 1.0, 0.0, 0.0);
	glRotatef(yangle, 0.0, 1.0, 0.0);
	glRotatef(zangle, 0.0, 1.0, 0.0);

    float GradientC;
    display_step = (display_step+1)%999999;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    FreshBodies();

    if(display_step%SHOW_INTERVAL!=0)
    {
        return;
    }

    for(int id=0;id<BODY_NUM;id++)
    {
        
        float GradientC= 1.0/TR_INTERVAL;
        for(int p=cur[id];;p=(p+1)%TR_INTERVAL)
        {
            GradientC+= 1.0/TR_INTERVAL;
            DrawSphere(track[id][p], GradientC, BODY_SIZE);

            if(p == (cur[id]+TR_INTERVAL-1)%TR_INTERVAL)
            {
                break;
            }
        }      

        if(SHOW_INFO) 
        {
            printf("===>>>Obj%d  x:%.2f y:%.2f z:%.2f vx:%.2f vy:%.2f vz:%.2f  ax:%.2f ay:%.2f az:%.2f\n",  
                    id, obj[id].pos.x, obj[id].pos.y, obj[id].pos.z,
                    obj[id].v.x, obj[id].v.y, obj[id].v.z,
                    obj[id].a.x, obj[id].a.y, obj[id].a.z); 
        }

    }
    
    if(SHOW_COORDINATE)
    {
        drawCoordinates();
    }

    glFlush();

}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN)
	{
		oldx = x;
		oldy = y;
	}
}

void motion(int x, int y)
{
	GLint deltax = oldx - x;
	GLint deltay = oldy - y;
	yangle += 360*(GLfloat)deltax / (GLfloat)WIDTH;
	xangle += 360*(GLfloat)deltay / (GLfloat)HEIGHT;
	zangle += 360*(GLfloat)deltay / (GLfloat)WIDTH;
	oldx = x;
	oldy = y;
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	WIDTH = w;
	HEIGHT = h;
	glViewport(0.0, 0.0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void KeyBoards(unsigned char key,int x,int y)
{
	switch (key)
	{
	case 'w':
		SHIFT_D--;
		glutPostRedisplay();
		break;
	case 's':
		SHIFT_D++;
		glutPostRedisplay();
		break;
	case 'a':
		SHIFT_R--;
		glutPostRedisplay();
		break;
	case 'd':
		SHIFT_R++;
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char *argv[])
{
    read_cfg();
    memset(cur,0,sizeof(int));
    FILE *f = fopen("/home/dynmi/Documents/Project/N-Body-Gravity-Simulation/inital_state.in","rb");
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    printf("Initial parameters:\n\n");
    fscanf(f,"%d",&BODY_NUM);
    int i=0;
    while(fscanf(f,"%f %f %f %f %f %f %f",
        &obj[i].m, &obj[i].pos.x, &obj[i].pos.y, &obj[i].pos.z,
        &obj[i].v.x, &obj[i].v.y, &obj[i].v.z)!=EOF)
    {
        printf("%f %f %f %f %f %f %f\n",
        obj[i].m, obj[i].pos.x, obj[i].pos.y, obj[i].pos.z,
        obj[i].v.x, obj[i].v.y, obj[i].v.z);
        i++;
    }
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    printf("\n\n");

    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Gravity Simulation");


    glEnable(GL_LIGHTING);   
    glEnable(GL_LIGHT0);     
    glEnable(GL_DEPTH_TEST); 

	glShadeModel(GL_SMOOTH);     
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutDisplayFunc(Display);
	glutIdleFunc(Display);
    glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(&KeyBoards);
	glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}