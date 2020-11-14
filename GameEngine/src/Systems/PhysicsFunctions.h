#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include "entityx/entityx.h"
#include "../Components/Components.h"
#include "../logger.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Physics
{
    bool DetectAABB(float x1, float y1, float width1, float height1,
    float x2, float y2, float width2, float height2){
        width1 = width1 / 2.0;
        width2 = width2 / 2.0;
        height1 = height1 / 2.0;
        height2 = height2 / 2.0;
        if (x1 + width1 > x2 - width2 &&
            x1 - width1 < x2 + width2 &&
            y1 + height1 > y2 - height2 &&
            y1 - height1 < y2 + height2) {
                return true;
            }
        return false;
    }

    glm::vec2 ClosestPointOnLineSegment(glm::vec2 A, glm::vec2 B, glm::vec2 Point){
        glm::vec2 AB = B - A;
        float t = glm::dot(Point - A, AB) / glm::dot(AB, AB);
        return A + min(max(t, 0.0f), 1.0f) * AB; // saturate(t) can be written as: min((max(t, 0), 1)
    }

    glm::vec2 ProjectionOnAnAxis(glm::vec2 line, glm::vec2 axis){
        return glm::dot(line, axis) / glm::dot(axis, axis) * axis;
    }

    float ScalarOfProjectionOnAxis(glm::vec2 proj, glm::vec2 axis){
        return glm::dot(proj, axis);
    }

    float minScalarOfProjectionsOnAxis(glm::vec2 TL, glm::vec2 TR, glm::vec2 BL, glm::vec2 BR, glm::vec2 axis){
        float tlProj = glm::dot(TL, axis);
        float trProj = glm::dot(TR, axis);
        float blProj = glm::dot(BL, axis);
        float brProj = glm::dot(BR, axis);

        float min = tlProj;
        if(trProj < min)
            min = trProj;
        if(blProj < min)
            min = blProj;
        if(brProj < min)
            min = brProj;
        
        return min;
    }

    float maxScalarOfProjectionsOnAxis(glm::vec2 TL, glm::vec2 TR, glm::vec2 BL, glm::vec2 BR, glm::vec2 axis){
        float tlProj = glm::dot(TL, axis);
        float trProj = glm::dot(TR, axis);
        float blProj = glm::dot(BL, axis);
        float brProj = glm::dot(BR, axis);

        float max = tlProj;
        if(trProj > max)
            max = trProj;
        if(blProj > max)
            max = blProj;
        if(brProj > max)
            max = brProj;
        
        return max;
    }

    //Circle - Circle
    bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        float c1PosX = c1->x + c1T->x;
        float c2PosX = c2->x + c2T->x;
        float c1PosY = c1->y + c1T->y;
        float c2PosY = c2->y + c2T->y;
        float distance = abs(sqrtf(powf(c2PosX - c1PosX, 2) + powf(c2PosY - c1PosY, 2)));
        if(distance <= c1->radius + c2->radius)
            return true;
        return false;
    }

    //Capsule - Capsule
    bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        //Capsule c1:
        float c1Rotation = c1T->angle * M_PI / 180.0f;
        glm::vec2 c1Tip = glm::vec2(c1->x, c1->y + (c1->a)/2 - c1->radius);
        c1Tip = glm::rotate(c1Tip, c1Rotation);
        c1Tip = glm::vec2(c1Tip.x + c1T->x, c1Tip.y + c1T->y);
        glm::vec2 c1Base = glm::vec2(c1->x, c1->y - (c1->a)/2 + c1->radius);
        c1Base = glm::rotate(c1Base, c1Rotation);
        c1Base = glm::vec2(c1Base.x + c1T->x, c1Base.y + c1T->y);
        glm::vec2 c1_A = c1Base;
        glm::vec2 c1_B = c1Tip;


        //Capsule c2:
        float c2Rotation = c2T->angle * M_PI / 180.0f;
        glm::vec2 c2Tip = glm::vec2(c2->x, c2->y + (c2->a)/2 - c2->radius);
        c2Tip = glm::rotate(c2Tip, c2Rotation);
        c2Tip = glm::vec2(c2Tip.x + c2T->x, c2Tip.y + c2T->y);
        glm::vec2 c2Base = glm::vec2(c2->x, c2->y - (c2->a)/2 + c2->radius);
        c2Base = glm::rotate(c2Base, c2Rotation);
        c2Base = glm::vec2(c2Base.x + c2T->x, c2Base.y + c2T->y);
        glm::vec2 c2_A = c2Base;
        glm::vec2 c2_B = c2Tip;

        // vectors between line endpoints:
        glm::vec2 v0 = c2_A - c1_A;
        glm::vec2 v1 = c2_B - c1_A; 
        glm::vec2 v2 = c2_A - c1_B; 
        glm::vec2 v3 = c2_B - c1_B;

        // squared distances:
        float d0 = glm::dot(v0, v0); 
        float d1 = glm::dot(v1, v1); 
        float d2 = glm::dot(v2, v2); 
        float d3 = glm::dot(v3, v3);

        // select best potential endpoint on capsule A:
        glm::vec2 bestA;
        if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
        {
            bestA = c1_B;
        }
        else
        {
            bestA = c1_A;
        }

        // select point on capsule B line segment nearest to best potential endpoint on A capsule:
        glm::vec2 bestB = ClosestPointOnLineSegment(c2_A, c2_B, bestA);
        // now do the same for capsule A segment:
        bestA = ClosestPointOnLineSegment(c1_A, c1_B, bestB);

        //Collision calc:
        glm::vec2 penetration_normal = bestA - bestB;
        float len = length(penetration_normal);
        penetration_normal /= len;  // normalize
        float penetration_depth = c1->radius + c2->radius - len;
        return penetration_depth > 0;
    }

    //Box - Circle
    bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        //Box stuff
        float boxPosX = c1->x + c1T->x;
        float boxPosY = c1->y + c1T->y;

        float boxRotation = c1T->angle * M_PI / 180.0f;

        //Circle
        float circleX = c2->x + c2T->x;
        float circleY = c2->y + c2T->y;

        //Rotate circle
        circleX = cosf(boxRotation) * (circleX - boxPosX) - sinf(boxRotation) * 
            (circleY - boxPosY) + boxPosX;
        circleY = sinf(boxRotation) * (circleX - boxPosX) + cosf(boxRotation) *
            (circleY - boxPosY) + boxPosY;


        //Collision stuff
        //Check first if center of the circle is inside the box:
        if(circleX > boxPosX - c1->width / 2 && circleX < boxPosX + c1->width / 2 &&
            circleY < boxPosY + c1->height / 2 && circleY > boxPosY - c1->height / 2)
            return true;
        
        //Now check if the circle is outside of the box
        float testX;
        float testY;
        if(circleX < boxPosX - c1->width / 2)
            testX = boxPosX - c1->width / 2;   //Testing left edge of box
        else if(circleX > boxPosX + c1->width / 2)
            testX = boxPosX + c1->width / 2;  //Testing right edge of box
        else
            testX = circleX;  //In this case the circle is between left and right edges
        
        if(circleY > boxPosY + c1->height / 2)
            testY = boxPosY + c1->height / 2;   //Testing top edge of box
        else if(circleY < boxPosY - c1->height / 2)
            testY = boxPosY - c1->height / 2;   //Testing bottom edge of box
        else
            testY = circleY;  //Circle is between the top and bottom edges;
        
        float distX = circleX - testX;
        float distY = circleY - testY;
        float distance = sqrtf(powf(distX, 2) + powf(distY, 2));

        if(distance < c2->radius)
            return true;
        return false;
    }

    //Circle - Box (Calls Box - Circle)
        bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
            return CheckCollision(c2, c1, c2T, c1T);
        }

    //Box - Capsule
    bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        //Box c1:
        glm::vec2 topLeft = glm::vec2(0 - c1->width / 2, 0 + c1->height / 2);
        glm::vec2 topRight = glm::vec2(0 + c1->width / 2, 0 + c1->height / 2);
        glm::vec2 bottomLeft = glm::vec2(0 - c1->width / 2, 0 - c1->height / 2);
        glm::vec2 bottomRight = glm::vec2(0 + c1->width / 2, 0 - c1->height / 2);
        //Rotate box corners and move to actual position:
        float theta = c1T->angle * M_PI / 180.0f;
        topLeft = glm::rotate(topLeft, theta);
        topLeft = glm::vec2(topLeft.x + c1T->x, topLeft.y + c1T->y);
        topRight = glm::rotate(topRight, theta);
        topRight = glm::vec2(topRight.x + c1T->x, topRight.y + c1T->y);
        bottomLeft = glm::rotate(bottomLeft, theta);
        bottomLeft = glm::vec2(bottomLeft.x + c1T->x, bottomLeft.y + c1T->y);
        bottomRight = glm::rotate(bottomRight, theta);
        bottomRight = glm::vec2(bottomRight.x + c1T->x, bottomRight.y + c1T->y);

        //Capsule c2:
        float c2Rotation = c2T->angle * M_PI / 180.0f;
        glm::vec2 c2Tip = glm::vec2(c2->x, c2->y + (c2->a)/2 - c2->radius);
        c2Tip = glm::rotate(c2Tip, c2Rotation);
        c2Tip = glm::vec2(c2Tip.x + c2T->x, c2Tip.y + c2T->y);
        glm::vec2 c2Base = glm::vec2(c2->x, c2->y - (c2->a)/2 + c2->radius);
        c2Base = glm::rotate(c2Base, c2Rotation);
        c2Base = glm::vec2(c2Base.x + c2T->x, c2Base.y + c2T->y);
        glm::vec2 c2_A = c2Base;
        glm::vec2 c2_B = c2Tip;

        //Test both ends of the capsule to all edges of the rectangle
        //Test the tip against the rect points:
        glm::vec2 tip2TL = topLeft - c2Tip;
        glm::vec2 tip2TR = topRight - c2Tip;
        glm::vec2 tip2BL = bottomLeft - c2Tip;
        glm::vec2 tip2BR = bottomRight - c2Tip;

        float d1T = glm::dot(tip2TL, tip2TL);
        float d2T = glm::dot(tip2TR, tip2TR);
        float d3T = glm::dot(tip2BL, tip2BL);
        float d4T = glm::dot(tip2BR, tip2BR);

        float testMin = d1T;
        if(testMin > d2T)
            testMin = d2T;
        if(testMin > d3T)
            testMin = d3T;
        if(testMin > d4T)
            testMin = d4T;

        glm::vec2 bestB = c2Tip;

        //Test the base against the rect points:
        glm::vec2 base2TL = topLeft - c2Base;
        glm::vec2 base2TR = topRight - c2Base;
        glm::vec2 base2BL = bottomLeft - c2Base;
        glm::vec2 base2BR = bottomRight - c2Base;

        float d1B = glm::dot(base2TL, base2TL);
        float d2B = glm::dot(base2TR, base2TR);
        float d3B = glm::dot(base2BL, base2BL);
        float d4B = glm::dot(base2BR, base2BR);

        if(testMin > d1B || testMin > d2B || testMin > d3B || testMin > d4B)
            bestB = c2Base;

        //Find the closest point on the rect to the selected capsule end point
        glm::vec2 closeTop = ClosestPointOnLineSegment(topLeft, topRight, bestB);
        glm::vec2 closeRight = ClosestPointOnLineSegment(topRight, bottomRight, bestB);
        glm::vec2 closeBottom = ClosestPointOnLineSegment(bottomLeft, bottomRight, bestB);
        glm::vec2 closeLeft = ClosestPointOnLineSegment(topLeft, bottomLeft, bestB);

        float minDistance = glm::length(closeTop - bestB);
        float bToRight = glm::length(closeRight - bestB);
        float bToBottom = glm::length(closeBottom - bestB);
        float bToLeft = glm::length(closeLeft - bestB);

        glm::vec2 bestA = closeTop;
        if(minDistance > bToRight){
            bestA = closeRight;
            minDistance = bToRight;
        }
        if(minDistance > bToBottom){
            bestA = closeBottom;
            minDistance = bToBottom;
        }
        if(minDistance > bToLeft)
            bestA = closeLeft;

        //Find the closest point on the capsule to the closest edge of the rect
        bestB = ClosestPointOnLineSegment(c2Tip, c2Base, bestA);

        //First rotate the test point with respect to the rect's rotation for easy calc:
        float boxPosX = c1->x + c1T->x;
        float boxPosY = c1->y + c1T->y;
        glm::vec2 bestBRotated = glm::vec2(cosf(theta) * (bestB.x - boxPosX) - sinf(theta) * 
            (bestB.y - boxPosY) + boxPosX, sinf(theta) * (bestB.x - boxPosX) + cosf(theta) *
            (bestB.y - boxPosY) + boxPosY);
        

        //Collision calc:
        //First test if the point on the capsule is inside the rect:
        if(bestBRotated.x > c1->x - c1->width / 2 && bestBRotated.x < c1->x + c1->width / 2 &&
            bestBRotated.y < c1->y + c1->height / 2 && bestBRotated.y > c1->y - c1->height / 2)
                return true;

        
        //Now do test for outside the rect with original points on capsule and rect:
        glm::vec2 penetration_normal = bestA - bestB;
        float len = length(penetration_normal);
        penetration_normal /= len;  // normalize
        float penetration_depth = c2->radius - len;
        return penetration_depth > 0;
    }

    //Circle - Capsule
    bool CheckCollision(ComponentHandle<CircleCollider>& c1, ComponentHandle<CapsuleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        //Capsule c2:
        float c2Rotation = c2T->angle * M_PI / 180.0f;
        glm::vec2 c2Tip = glm::vec2(c2->x, c2->y + (c2->a)/2 - c2->radius);
        c2Tip = glm::rotate(c2Tip, c2Rotation);
        c2Tip = glm::vec2(c2Tip.x + c2T->x, c2Tip.y + c2T->y);
        glm::vec2 c2Base = glm::vec2(c2->x, c2->y - (c2->a)/2 + c2->radius);
        c2Base = glm::rotate(c2Base, c2Rotation);
        c2Base = glm::vec2(c2Base.x + c2T->x, c2Base.y + c2T->y);
        glm::vec2 c2_A = c2Base;
        glm::vec2 c2_B = c2Tip;

        //Circle c1:
        glm::vec2 c1_Point = glm::vec2(c1->x + c1T->x, c1->y + c1T->y);

        //Selecting point on the capsule closest to the circle:
        glm::vec2 c2_Best = ClosestPointOnLineSegment(c2_A, c2_B, c1_Point);

        //Collision calculation:
        glm::vec2 penetration_normal = c1_Point - c2_Best;
        float len = length(penetration_normal);
        penetration_normal /= len;  // normalize
        float penetration_depth = c1->radius + c2->radius - len;
        return penetration_depth > 0;
    }

    //Box - Box
    bool CheckCollision(ComponentHandle<BoxCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        
        //Box 1 Setup
        glm::vec2 b1TopLeft = glm::vec2(0 - c1->width / 2, 0 + c1->height / 2);
        glm::vec2 b1TopRight = glm::vec2(0 + c1->width / 2, 0 + c1->height / 2);
        glm::vec2 b1BottomLeft = glm::vec2(0 - c1->width / 2, 0 - c1->height / 2);
        glm::vec2 b1BottomRight = glm::vec2(0 + c1->width / 2, 0 - c1->height / 2);
        //Rotate box corners and move to actual position:
        float b1Theta = c1T->angle * M_PI / 180.0f;
        b1TopLeft = glm::rotate(b1TopLeft, b1Theta);
        b1TopLeft = glm::vec2(b1TopLeft.x + c1T->x, b1TopLeft.y + c1T->y);
        b1TopRight = glm::rotate(b1TopRight, b1Theta);
        b1TopRight = glm::vec2(b1TopRight.x + c1T->x, b1TopRight.y + c1T->y);
        b1BottomLeft = glm::rotate(b1BottomLeft, b1Theta);
        b1BottomLeft = glm::vec2(b1BottomLeft.x + c1T->x, b1BottomLeft.y + c1T->y);
        b1BottomRight = glm::rotate(b1BottomRight, b1Theta);
        b1BottomRight = glm::vec2(b1BottomRight.x + c1T->x, b1BottomRight.y + c1T->y);
        //Box 2 Setup
        glm::vec2 b2TopLeft = glm::vec2(0 - c2->width / 2, 0 + c2->height / 2);
        glm::vec2 b2TopRight = glm::vec2(0 + c2->width / 2, 0 + c2->height / 2);
        glm::vec2 b2BottomLeft = glm::vec2(0 - c2->width / 2, 0 - c2->height / 2);
        glm::vec2 b2BottomRight = glm::vec2(0 + c2->width / 2, 0 - c2->height / 2);
        //Rotate box corners and move to actual position:
        float b2Theta = c2T->angle * M_PI / 180.0f;
        b2TopLeft = glm::rotate(b2TopLeft, b2Theta);
        b2TopLeft = glm::vec2(b2TopLeft.x + c2T->x, b2TopLeft.y + c2T->y);
        b2TopRight = glm::rotate(b2TopRight, b2Theta);
        b2TopRight = glm::vec2(b2TopRight.x + c2T->x, b2TopRight.y + c2T->y);
        b2BottomLeft = glm::rotate(b2BottomLeft, b2Theta);
        b2BottomLeft = glm::vec2(b2BottomLeft.x + c2T->x, b2BottomLeft.y + c2T->y);
        b2BottomRight = glm::rotate(b2BottomRight, b2Theta);
        b2BottomRight = glm::vec2(b2BottomRight.x + c2T->x, b2BottomRight.y + c2T->y);

        //Set up the axis for projection from both boxes
        glm::vec2 b1Axis1 = b1TopRight - b1TopLeft;
        glm::vec2 b1Axis2 = b1TopRight - b1BottomRight;

        glm::vec2 b2Axis1 = b2TopRight - b2TopLeft;
        glm::vec2 b2Axis2 = b2TopLeft - b2BottomLeft;

        //Get min and max scalar representatives for each point onto each axis:
        //Axis b1a1:
        float b1min = minScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b1Axis1);
        float b1max = maxScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b1Axis1);
        float b2min = minScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b1Axis1);
        float b2max = maxScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b1Axis1);

        if(!(b2min <= b1max && b2max >= b1min))
            return false;

        //Axis b1a2:
        b1min = minScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b1Axis2);
        b1max = maxScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b1Axis2);
        b2min = minScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b1Axis2);
        b2max = maxScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b1Axis2);

        if(!(b2min <= b1max && b2max >= b1min))
            return false;

        //Axis b2a1:
        b1min = minScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b2Axis1);
        b1max = maxScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b2Axis1);
        b2min = minScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b2Axis1);
        b2max = maxScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b2Axis1);

        if(!(b2min <= b1max && b2max >= b1min))
            return false;

        //Axis b2a2:
        b1min = minScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b2Axis2);
        b1max = maxScalarOfProjectionsOnAxis(b1TopLeft, b1TopRight, b1BottomLeft, b1BottomRight, b2Axis2);
        b2min = minScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b2Axis2);
        b2max = maxScalarOfProjectionsOnAxis(b2TopLeft, b2TopRight, b2BottomLeft, b2BottomRight, b2Axis2);

        if(!(b2min <= b1max && b2max >= b1min))
            return false;
        
        return true;
    }

    //Capsule - Box (Calls Box - Capsule)
    bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<BoxCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        return CheckCollision(c2, c1, c2T, c1T);
    }

    //Capsule - Circle (Calls Circle - Capsule)
    bool CheckCollision(ComponentHandle<CapsuleCollider>& c1, ComponentHandle<CircleCollider>& c2, ComponentHandle<Transform> c1T, ComponentHandle<Transform> c2T) {
        return CheckCollision(c2, c1, c2T, c1T);
    } 
    

}